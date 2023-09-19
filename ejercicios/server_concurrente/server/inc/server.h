/* Author: Lucas Liaño (2023)
 * 
 * 
 * Concurrent server functions.
 * 
*/

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

typedef struct {
    int socket;
    pid_t pid;
} child_t;

int main_child(int socketChild);
void remove_pid(int pid);
void sort_pid_list (void);
void sigchld_handler();
void print_child_table(void);
int setup_sockaddr(struct sockaddr_in* data, unsigned int family, unsigned int port);
void configure_socket(int* socketFD, unsigned int port);
void exit_on_error(const char* msg);


#endif /* defined(__SERVER_H) */