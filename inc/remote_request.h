#ifndef REMOTE_REQUEST_H
#define REMOTE_REQUEST_H

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "logger.h"

#define PORT_NO (6000)
// #define MAX_BUFFER_SIZE (100) 
#define LISTEN_MAX (10)

extern const char *log_level[10];

pthread_t remote_request_thread;

char message[MAX_BUFFER_SIZE];
int new_socket, server_socket;

void *remote_request_callback(void *);

int socket_creation_server(int);


#endif