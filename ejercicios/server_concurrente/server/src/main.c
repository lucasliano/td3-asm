#include "server.h"
#include "mpu6050.h"

// --- Data used for more than one process ---
int my_pid;             // Each process will update this variable

// Child manager
child_t* childList;     // Array with all the childs PIDs (Global so it can be accessed in the signals handlers)
                        // NOTE: This vector, no matter what, will contain the process childs.
int currChildQty = 0;   // Number of current childs (of each process)

// Server Data
int socketParent, socketChild;  // Sockets' File Descriptors (FD)
struct sockaddr_in cli_addr;    // Client address

// Inter-Process Communication Variables
// TODO: Check how shared memory is working
sem_t* sem_producer;
int shmid;
char *shm_data;

// Configuration file
configParams_t configs;

// Cleanup function
void (*clean_and_exit)(int) = exit;   // This is a common function to exit different processes.


int main(void)
{
    // --- Server init ---
    // system("clear");
    printf("Starting server...\n");
    printf("Parent-PRODUCER PID: (%d)\n", my_pid = getpid());

    // Loads Config information
    get_config(&configs);

    // Producer (Oldest parent) will only have 1 child, the server.
    childList = (child_t*) calloc(1,  sizeof(child_t));

    // --- Signal trapping ---
    signal(SIGCHLD, sigchld_handler );      // Signal handler - Captures SIGCHLD to terminate child tasks
    signal(SIGPIPE, sigpipe_handler );      // Signal handler - Captures SIGPIPE to detect broken pipes
    signal(SIGINT,  sigint_handler);        // Signal handler - Captures SIGINT  to end the program
    signal(SIGUSR2, sigusr2_handler);       // Signal handler - Captures SIGUSR2 to update the server params

    // --- Synchronization Mechanism ---
    if (((sem_producer = sem_open(SEM_PRODUCER, O_CREAT, 0660, 1)) == SEM_FAILED))
        exit_on_error("Could not create semaphore."); 
    

    // Fork to create the producer and consumer(s) processes.
    fflush(stdout);
    switch (childList[currChildQty].pid = fork()) 
    {
        case -1:
            exit_on_error("Error while forking! Ending server session..");
            break;

        case 0:
            // Executes the server (child) routine
            clean_and_exit = exit_server;
            my_pid = getpid();
            printf("SERVER (%d): Im the new child.\n", my_pid);
            main_parent_server();
            break;

        default:
            // Parent-producer routine
            clean_and_exit = exit_producer;
            printf("PRODUCER (%d): Starting producers routine..\n", my_pid);
            currChildQty++;
            print_child_table();
            main_parent_producer();
            break;
    }
    

    // Should never get here.
    clean_and_exit(0);
    return 0;
}

void main_parent_producer (void)
{
    /*******************************************************************************
    * Main parent producer process. This parent will have only one child, the server. 
    * 
    * @return None. Should always run unless is killed.
    ******************************************************************************/

    key_t key;
    char buffer[SHM_SIZE];          // Buffer that we will read
    int buffCnt, bytesRead;

    // --- Creates/attachs the shared memory resource ---
    key = ftok(CLAVE_SHM, 'A');     // Generates unique key
    
    if ((shmid = shmget(key, sizeof(buffer), 0666|IPC_CREAT)) < 0)  // Creates the shm
        exit_on_error("Error while running shmget().");

    printf("shmid_prod: %d\n", shmid);

    if(*(shm_data = shmat(shmid, NULL, 0)) < 0)     // Connects the shm to the task
        exit_on_error("Error while running shmat().");


    // --- Main producer loop ---
    while(1)
    {
        buffCnt = 0;
        bytesRead = 0;

        // Fills the buffer
        while ( buffCnt < SHM_SIZE)
        {

            while((bytesRead = read_sensor(buffer + buffCnt, configs.coefFilter)) <= 0)
                usleep(10000); // Retries on error

            buffCnt += bytesRead;
            // usleep(50000);     // Delay to reduce processing
        }

        // Copies the buffer to the shared memory
        if((sem_wait(sem_producer)) < 0)                // Takes the semaphore
            exit_on_error("Error while waiting semaphore.");

        memcpy(shm_data, buffer, sizeof(buffer));       // Copies data into shm

        if((sem_post(sem_producer)) < 0) 
            exit_on_error("Error while unlocking semaphore."); // Unlocks the semaphore
        
        
        usleep(DELAY_PRODUCER);     // Delay to reduce processing
    }


    exit_on_error("main_parent_producer(): Should never get here!\n");
    return;
}

void main_parent_server (void)
{
    /*******************************************************************************
    * This is the parent-server main routine.
    * 
    * @return None.
    ******************************************************************************/
    socklen_t sizeCli_addr = sizeof(cli_addr);

    // --- Initiallization ---
    configure_socket(&socketParent, configs.port);
    printf("SERVER (%d): Using PORT %d.\n", my_pid, configs.port);

    listen(socketParent, configs.backlog);  // Prepares to accept up to configs.backlog before starts refusing new ones
    printf("SERVER (%d): Waiting connections...\n", my_pid);

    // --- Server main process ---
    while (1) 
    {
        
        if(currChildQty >= configs.maxConnections) // When we cannot handle more connections we usleep for 10ms
        {
            usleep(100000);
            continue;
        } 

        // Otherwise we accept the new request
        socketChild = accept(socketParent, (struct sockaddr *) &cli_addr, &sizeCli_addr);
        if(socketChild < 0 )
        {
            printf("SERVER (%d): Could not accept an incomming connection. Skipping..", my_pid);
            continue;
        }

        // Once the request was accepted we ask for more child space
        childList = (child_t*) realloc(childList, (currChildQty+1) * sizeof(child_t));    // More space for childs

        // When the connection was made we fork to handle the processing in another task.
        fflush(stdout);
        switch (childList[currChildQty].pid = fork()) 
        {
            case -1:
                exit_on_error("Error while forking! Ending server session..");
                break;

            case 0:
                // Executes the child routine
                clean_and_exit = exit_child;
                my_pid = getpid();
                currChildQty = 0;
                printf("CHILD (%d): Im the new child.\n", my_pid);
                printf("CHILD (%d): New client was accepted: %s \n", my_pid, inet_ntoa(cli_addr.sin_addr));

                close(socketParent);
                main_child(socketChild);
                break;

            default:
                // Parent routine
                printf("SERVER (%d): CHILD (%d) accepted one connection from IP (%s)\n", my_pid, childList[currChildQty].pid, inet_ntoa(cli_addr.sin_addr));
                currChildQty++;
                close(socketChild);
                print_child_table();
                break;
        }
    }

    // Should never get here.
    clean_and_exit(0);
    return;
}

void main_child(int socketChild)
{
    /*******************************************************************************
    * This is the child main routine. 
    * 
    * @param  socketChild Socket is used to communicate with the client.
    * 
    * @return None.
    ******************************************************************************/

    int numbytes, sem_libre = 0;
    char word[2] = {'O', 'K'};
    key_t key;
    int buffer_index;
    int fsm_state;


    // --- Creates/attachs the shared memory resource ---
    key = ftok(CLAVE_SHM, 'A');     // Generates unique key
    
    if ((shmid = shmget(key, SHM_SIZE, 0666|IPC_CREAT)) < 0)  // Creates the shm
        exit_on_error("Error while running shmget().");

    printf("shmid: %d\n", shmid);

    if(*(shm_data = shmat(shmid, NULL, 0)) < 0)     // Connects the shm to the task
        exit_on_error("Error while running shmat().");


    // --- Starts comms with client ---
    if ( ( numbytes = send(socketChild, word, sizeof(word), 0) ) == -1 )
        exit_on_error("send data port");

    if ( ( numbytes = recv(socketChild, word, sizeof(word), 0) ) == -1 )  
        exit_on_error("recv data port");

    // TODO: Estamos haciendo OK->AK->OK.. Medio raro
    // Checks for ACK
    if(word[0] != 'A' || word[1] != 'K')
        clean_and_exit(0);

    // Sends OK
    memcpy(word, "OK", 2);
    if ( ( numbytes = send(socketChild, word, sizeof(word), 0) ) == -1 ) 
        exit_on_error("send data port");


    // Main FSM
    fsm_state = CHILD_FSM_ESPERANDO;
    while(1)
    {
        sem_getvalue(sem_producer, &sem_libre);
        // printf("CHILD (%d): State: %d\n", my_pid, fsm_state);
        switch (fsm_state)
        {
            case CHILD_FSM_ESPERANDO:
                // // TODO: Ver de poner dos partes de shmem
                // // Si ya me leí todos los datos, espero a que el producer actualicé
                // if (buffer_index >= SHM_SIZE)
                // {
                //     // printf("CHILD_FSM_ESPERANDO - Buffer lleno\n");
                //     if (!sem_libre)
                //         buffer_index = 0;
                // }else{
                //     // Si el semaforo está ocupado, espero a que se libere.
                //     printf("CHILD_FSM_ESPERANDO - Semáforo ocupado\n");
                //     if (sem_libre)
                //         fsm_state = CHILD_FSM_ENVIANDO;
                // }

                if (sem_libre)
                {
                    buffer_index = 0;
                    fsm_state = CHILD_FSM_ENVIANDO;
                }
                break;
            
            case CHILD_FSM_ENVIANDO:
                if (!sem_libre || buffer_index >= SHM_SIZE)
                {
                    fsm_state = CHILD_FSM_ESPERANDO;
                    usleep(DELAY_PRODUCER);
                }else{
                    if ( ( numbytes = recv(socketChild, word, sizeof(word), 0) ) == -1 )    // Keepalive
                        exit_on_error("recv data port");

                    if(word[0]=='K' && word[1]=='A')
                    {
                        //Envio el dato al cliente
                        if ( ( numbytes = send(socketChild, shm_data + buffer_index, PACKET_SIZE, 0) ) < 0) 
                            exit_on_error("send data port");
                        
                        buffer_index += numbytes;
                    } 
                    else if(word[0]=='E' && word[1]=='N')
                    {
                        clean_and_exit(0);
                    }
                }
                break;

            default:
                // Should never get here.
                clean_and_exit(0);
                break;
        }
    }

    // Should never get here.
    printf("CHILD (%d): Exits before ACK was received.\n", my_pid);
    clean_and_exit(0);
}

void sigint_handler(int s)
{
    /*******************************************************************************
    * Handler to propagate the SIGINT signal to all childs.
    * 
    * 
    * @return None.
    ******************************************************************************/
    clean_and_exit(0);
}

void sigchld_handler(int s)
{
    /*******************************************************************************
    * Handler to kill childs after receiving SIGCHLD.
    * 
    * 
    * @return None.
    ******************************************************************************/

    signal(SIGCHLD, sigchld_handler);     // We need to reasign the signal after being called

    int status;
    int child_pid = waitpid(-1, &status, WNOHANG);

    do
    {
        if(child_pid == -1)
            exit_on_error("Error while trying to kill child! This message shouldn't be displayed! There might be a memory leak. \n");

        remove_pid(child_pid);

        printf("PARENT (%d): Child (%d) [status: 0x%2.2X] was killed.\n", my_pid, child_pid, WTERMSIG(status));
        child_pid = waitpid(-1, NULL, WNOHANG);
    } while (child_pid > 0);

    print_child_table();

    return;
}

void sigpipe_handler(int s)
{
    /*******************************************************************************
    * Handler to trap SIGPIPE signal. This will happen when the client closes
    * the connection.
    * 
    * 
    * @return None. Will exit the child process.
    ******************************************************************************/

    printf("\n\nCHILD (%d): Broken PIPE. Exiting..\n\n", my_pid);
    exit_child(0);
    return;
}

void sigusr2_handler(int s)
{

    // Depending on type of process we do different stuff
    if (clean_and_exit == exit_producer)
    {
        printf("PRODUCER (%d): Sending SIGUSR2 to server..\n", my_pid);

        for(int i = 0; i < currChildQty; i++)
        {
            if(childList[i].pid != 0)
            {
                kill(childList[i].pid, SIGUSR2);
            }
        }
        reload(&configs);
    }else if (clean_and_exit == exit_server)
    {   
        reload(&configs);
        printf("SERVER (%d): New max connection limit was stablished to: %d\n", my_pid, configs.maxConnections);
    } // Other processes ignore this.
}

// -------- Auxiliary functions --------

void remove_pid(int pid)
{
    /*******************************************************************************
    * Remove the provided process from the child_t list.
    * 
    * @param pid Process ID to be removed.
    * 
    * @return None.
    ******************************************************************************/

    for(int i = 0; i < currChildQty; i++)
    {
        if(childList[i].pid == pid)
        {
            childList[i].pid = 0;
            sort_pid_list();
            currChildQty--;
            childList = (child_t*) realloc(childList, (currChildQty) * sizeof(child_t));    // Update space
            return;
        }
    }
}

void sort_pid_list (void)
{
    /*******************************************************************************
    * Sorts the child_t list.
    * 
    * @return None.
    ******************************************************************************/
    for(int i = 0; i < currChildQty; i++)
    {
        for(int j  = i; j > 0 ; j--)
        {
            if(childList[j].pid > childList[j-1].pid)
            {
                child_t aux = childList[j];
                childList[j] = childList[j-1];
                childList[j-1] = aux;
            }
        }
    }
}

void print_child_table(void)
{
    /*******************************************************************************
    * Prints table with currently used connections.
    * 
    * @return None.
    ******************************************************************************/
    if (clean_and_exit == exit_producer)
    {
        printf("---------- PRODUCER (%d) -------------\n", my_pid);
    }else if (clean_and_exit == exit_server)
    {
        printf("----------- SERVER (%d) --------------\n", my_pid);
    }else{
        printf("----------- PARENT (%d) --------------\n", my_pid);
    }


    if (currChildQty == 0) printf("This process does not have childs.\n");
    
    for(int i = 0; i < currChildQty; i++)
    {
        printf("childList[%d].pid: %d\n",i,childList[i].pid );
    }

    printf("-----------------------------------------\n");

    return;
}

int setup_sockaddr(struct sockaddr_in* data, unsigned int family, unsigned int port)
{
    /*******************************************************************************
    * Simple function to configure the sockaddr structure.
    * 
    * @param data Structure to be filled.
    * @param family Family of socket. This will usually be AF_INET.
    * @param port System port that will be used. This port must not be used by other service.
    * 
    * @return None.
    ******************************************************************************/


    data->sin_family = family;
    data->sin_port = htons(port);           // htons = Host To Network Short
    data->sin_addr.s_addr = INADDR_ANY;     // INADDR_ANY: Address to accept any incomming address
    memset((void*)&(data)->sin_zero, 0, 8); // Fill w/ zeros

    return 0;
}

void configure_socket(int* socketFD, unsigned int port)
{
    /*******************************************************************************
    * Creates a socket with the following configuration:
    *   AF_INET: IPv4
    *   SOCK_STREAM: TCP/IP
    *   SubProtocol: None
    * 
    * @param socketFD File descriptor used to create the socket.
    * @param port System port that will be used. This port must not be used by other service.
    * 
    * @return None but the program will exit on error.
    ******************************************************************************/

    struct sockaddr_in serv_addr;

    *socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (*socketFD < 0) 
        exit_on_error("Error while creating parent socket!");
    
    if (setup_sockaddr(&serv_addr, AF_INET, port) < 0) 
        exit_on_error("Error while configuring sockaddr!");
    
    if (bind(*socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        exit_on_error("Error while binding!");

    return;
}

void exit_on_error(const char* msg)
{
    /*******************************************************************************
    * Prints a message and then exits.
    * 
    * @param msg String to be printed befor exiting.
    * 
    * @return None. The program will exit.
    ******************************************************************************/
    printf("ERROR: %s", msg);
    clean_and_exit(0);
}

void exit_child(int s)
{
    /*******************************************************************************
    * Exits the child program after closing resources.
    * 
    * @return None. The program will exit.
    ******************************************************************************/
    shmdt(shm_data);                // Disconnect shared memory
    sem_close(sem_producer);        // Closes semaphore
    sem_unlink(SEM_PRODUCER);       // Unlinks the semaphore
    close(socketChild);             // Closes child socket
    close(socketParent);            // Closes parent socket
    free(childList);                // Frees the vector.
    exit(s);
}

void exit_producer(int s)
{
    /*******************************************************************************
    * Exits the parent-producer after closing resources and killing childs.
    * 
    * @return None. The program will exit.
    ******************************************************************************/
    
    // Makes sure all childs die.
    for(int i = 0; i < currChildQty; i++)
    {
        if(childList[i].pid != 0)
        {
            kill(childList[i].pid, SIGINT);
        }
    }

    // Close all resources
    sem_close(sem_producer);        // Closes semaphore
    sem_unlink(SEM_PRODUCER);       // Unlinks the semaphore
    shmdt(shm_data);                // Disconnect shared memory
    shmctl(shmid, IPC_RMID, NULL);  // Deletes shared memory
    free(childList);                // Frees the vector.

    // End
    printf("Resources were released successfully\n");
    exit(s);
}

void exit_server(int s)
{
    /*******************************************************************************
    * Exits the parent-server after closing resources and killing childs.
    * 
    * @return None. The program will exit.
    ******************************************************************************/
    
    // Makes sure all childs die.
    for(int i = 0; i < currChildQty; i++)
    {
        if(childList[i].pid != 0)
        {
            kill(childList[i].pid, SIGINT);
        }
    }
    
    sem_close(sem_producer);        // Closes semaphore
    sem_unlink(SEM_PRODUCER);       // Unlinks the semaphore
    close(socketChild);             // Closes child socket
    close(socketParent);            // Closes parent socket
    free(childList);                // Frees the vector.
    exit(s);
}

void get_config(configParams_t* configs)
{
    /*******************************************************************************
    * Gets configuration information from childs.
    * 
    * @return None..
    ******************************************************************************/
    // TODO: Translate!
    
    FILE *fp;
    char linea[1024];
    int i = 0;
    char * filtroStr;
    char * token;

    if((fp = fopen(PATH_CONFIG,"r"))){
        while(!feof(fp))
        {
            fscanf(fp, "port: %d\r\nmaxConn: %d\r\nbacklog: %d\r\nfilter: ",
                   &configs->port, &configs->maxConnections, &configs->backlog);

            // Ahora procesamos la línea de filtro que contiene números double
            filtroStr = fgets(linea, sizeof(linea), fp); // Leer la línea de filtro
            token = strtok(filtroStr, ",");
            while (token != NULL && i < MAX_COEF)
            {
                sscanf(token, "%lf", &configs->coefFilter[i]);
                //printf("Coef %d) %.17lf\n", i, configs->coefFilter[i]);
                token = strtok(NULL, ",");
                i++;
            }
        }
        fclose(fp);

        printf("PORT: %d\n", configs->port);
        printf("maxConnections: %d\n", configs->maxConnections);
        printf("backlog: %d\n", configs->backlog);
    }else{
        printf("No configuration file was found! Using default values.\n");
        double coef[] = DEFAULT_COEF;
        int num_coef;

        configs->port           = DEFAULT_PORT;
        configs->maxConnections = DEFAULT_MAX_CONNECTIONS;
        configs->backlog        = DEFAULT_MAX_BACKLOG;

        num_coef = sizeof(coef) / sizeof(coef[0]);
        for (int i = 0; i < num_coef; i++) {
            configs->coefFilter[i] = coef[i];
        }
    }
    
}

void reload(configParams_t * configs)
{
    /*******************************************************************************
    * Reloads the configuration.
    * 
    * @return None..
    ******************************************************************************/
    // TODO: Translate!

    FILE *fp;
    int temp;
    char linea[1024];
    char * filtroStr;
    char * token;
    int i = 0;
    if((fp = fopen(PATH_CONFIG,"r")))
    {
        while(!feof(fp))
        {
            fscanf(fp,"port: %d\r\nmaxConn: %d\r\nbacklog: %d\r\nfilter: ",
            &temp, &configs->maxConnections, &temp);
            // Ahora procesamos la línea de filtro que contiene números double
            filtroStr = fgets(linea, sizeof(linea), fp); // Leer la línea de filtro
            token = strtok(filtroStr, ",");
            while (token != NULL && i < MAX_COEF)
            {
                sscanf(token, "%lf", &configs->coefFilter[i]);
                // printf("Coef %d) %.17lf\n", i, configs->coefFilter[i]);
                token = strtok(NULL, ",");
                i++;
            }
        }
        fclose(fp);
    }else{
        printf("No configuration file was found! Using default values.\n");
    }
}

