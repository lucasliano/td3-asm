#ifndef __SERVER_H
#define __SERVER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <signal.h>         // Signal handling
#include <errno.h>          // Used in sigpipe_handler()
#include "mpu6050.h"

// Defines
#define CLAVE_SHM           "MYCLAVE"
#define SHM_SIZE            100
#define SEM_PRODUCER        "SYNC_SEM"


#define CHILD_FSM_ESPERANDO 0
#define CHILD_FSM_ENVIANDO  1
#define DELAY_PRODUCER 100000

// Defaults
#define PATH_CONFIG         "./config.txt"
#define MAX_COEF            12
#define DEFAULT_MAX_BACKLOG 10
#define DEFAULT_COEF        {0.000016619229044057786,0.00018281151948463563,0.0009140575974231782,0.0027421727922695345,0.00548434558453907,0.007678083818354697,0.007678083818354697,0.005484345584539069,0.0027421727922695345,0.0009140575974231782,0.00018281151948463563,0.000016619229044057786}
#define DEFAULT_PORT 5555
#define DEFAULT_MAX_CONNECTIONS 3

// Colors
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef struct {
    int socket;
    pid_t pid;
} child_t;


typedef struct
{
    int    port;
    int    maxConnections;
    int    backlog;
    double coefFilter[MAX_COEF];
}configParams_t;

// Handlers
void sigchld_handler(int);
void sigpipe_handler(int);
void sigint_handler(int);
void sigusr2_handler(int);

// Forks
void main_child(int socketChild);
void main_parent_server(void);
void main_parent_producer(void);


// Child related
void remove_pid(int pid);
void sort_pid_list (void);
void print_child_table(void);

// Cleanup
void exit_on_error(const char* msg);
void exit_child(int s);
void exit_producer(int s);
void exit_server(int s);

// Server related
int setup_sockaddr(struct sockaddr_in* data, unsigned int family, unsigned int port);
void configure_socket(int* socketFD, unsigned int port);
void get_config(configParams_t* configs);
void reload(configParams_t * configs);

// Filter
void filter_neon(const float *coef, const float *entrada, float *salida, int longitud);

#endif /* __SERVER_H */