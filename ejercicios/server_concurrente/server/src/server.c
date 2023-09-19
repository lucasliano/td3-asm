#include "../inc/server.h"

#define PORT 5050
#define MAX_CONNECTIONS 3

child_t childList[MAX_CONNECTIONS]; // lista con todos los PID de los hijos (Global para que puedan acceder desde Signals)
int cantConexionesActuales = 0;     // numero de conexiones actuales aceptadas


int main(void)
{
    int status = 1; // Main program status. Program will exit when status = 0;

    int socketParent, socketChild;              // Sockets' File Descriptors (FD)

    struct sockaddr_in cli_addr;                // Dirección del child
    socklen_t sizeCli_addr = sizeof(cli_addr);  // Tamaño en bytes de la direccion cliente
                                                // Notese, no se usa lo mismo para la direccion
                                                // del server, porque bind() no pide puntero al tamaño
                                                // pero accept() si lo pide.

    // --- Server init ---
    printf("Starting server...\n");

    signal(SIGCHLD, sigchld_handler );      // Signal handler - Captures SIGCHLD signal to terminate child tasks

    configure_socket(&socketParent, PORT);
    printf("Socket was successfully created.\n");

    listen(socketParent, MAX_CONNECTIONS);  // Prepares to accept up to MAX_CONNECTIONS before starts refusing new ones
    printf("Waiting connections...\n");
    

    // --- Server main process ---
    while (status) 
    {
        
        if(cantConexionesActuales >= MAX_CONNECTIONS) // When we cannot handle more connections we sleep for 10ms
        {
            sleep(0.01);
            continue;
        } 
        
        // Otherwise we accept the new request
        socketChild = accept(socketParent, (struct sockaddr *) &cli_addr, &sizeCli_addr);
        if(socketChild < 0 )
        {
            printf("PARENT: Could not accept an incomming connection. Skipping..");
            continue;
        }

        // When the connection was made we fork to handle the processing in another task.
        switch (childList[cantConexionesActuales].pid = fork()) 
        {
            case -1:
                exit_on_error("Error while forking! Ending server session..");
                break;

            case 0:
                // Executes the child routine
                printf("CHILD: Im the child w/ PID: (%d)\n", getpid());
                printf("CHILD: New client was accepted: %s \n", inet_ntoa(cli_addr.sin_addr));

                status = main_child(socketChild);
                break;

            default:
                // Parent routine
                cantConexionesActuales++;
                close(socketChild);
                printf("PARENT: El pid: '(%d)' acepto una solicitud de (%s)\n", childList[cantConexionesActuales].pid, inet_ntoa(cli_addr.sin_addr));
                print_child_table();
                break;
        }
    }

    // After status = 0
    close(socketParent);
    printf("Ending session successfully! :D\n");

    return 0;
}

int main_child(int socketChild)
{
    /*******************************************************************************
    * This is the child main routine.
    * 
    * @param socketChild Socket used to communicate with the client.
    * 
    * @return Zero when finished. This return shall be used to stop the main 
    *         while(status) of the task.
    ******************************************************************************/

    int writeStatus, variableLectura;
    char str[100]; 

    writeStatus = write(socketChild, "Hello World!", 12);
    if (writeStatus < 0)
    {
        exit_on_error("CHILD: ERROR: Writing to socket fail");
    }else{
        printf("CHILD: Message sent!\n");
    }

    for (int i = 0; i < 5; i++)
    {
        sprintf(str, "Sending message number N°%d\n", i);
        writeStatus = write(socketChild, str, sizeof(str));
        if (writeStatus < 0)
        {
            exit_on_error("CHILD: ERROR: Writing to socket fail");
        }else{
            printf("CHILD: Message sent!\n");
        }
        sleep(1);
    }


    // FIXME: Esta buggeadisisisisissmo. No entiende que del otro lado cerraron la comms.

    printf("CHILD: Reading...!\n");

    memset((void *) str, 0, 100);       //Vaciamos el buffer para poder leer
    variableLectura = read(socketChild,str,100);

    if (variableLectura < 0){
        exit_on_error("CHILD: ERROR: Could not read correctly!");
    }else{
        printf("CHILD: Recibi esto: %s\n",str);
    }

    printf("CHILD: Closing comms.\n");
    close(socketChild);

    return 0;
}

void sigchld_handler()
{
    /*******************************************************************************
    * Handler to kill childs after receiving SIGCHLD.
    * 
    * 
    * @return None.
    ******************************************************************************/

    signal(SIGCHLD, sigchld_handler);     // We need to reasign the signal after being called

    int child_pid = wait(NULL);
    if(child_pid == -1)
        exit_on_error("Error while trying to kill child!");


    remove_pid(child_pid);
    sort_pid_list();
    printf("PARENT: Child (%d) was killed.\n", child_pid);

    return;
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

    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if(childList[i].pid == pid)
        {
            childList[i].pid = 0;
            cantConexionesActuales--;
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
    for(int i = 0; i < MAX_CONNECTIONS; i++)
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

    printf("--------------------------------------\n");
    for(int i = 0; i < MAX_CONNECTIONS; i++)
    {
        printf("childList[%d].pid: %d\n",i,childList[i].pid );
    }
    printf("--------------------------------------\n");

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
    perror(msg);
    exit(0);
}

