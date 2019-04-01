#ifndef REMOTE_REQUEST_H
#define REMOTE_REQUEST_H

/**
 * @\file	remote_request.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for remote request thread spawn
 * @\date	03/30/2019
 *
 */

/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "lux_wrapper.h"
#include "temp.h"
#include "logger.h"
#include "common.h"

/*****************************************************************
						Macros
*****************************************************************/
#define PORT_NO (6001)
#define LISTEN_MAX (10)

/*****************************************************************
						Globals
*****************************************************************/
extern const char *log_level[10];

pthread_t remote_request_thread;

char message[MAX_BUFFER_SIZE];
int new_socket, server_socket;

extern int temp_dead_flag;
extern int lux_dead_flag;

/*****************************************************************
						Function Protoypes
*****************************************************************/
/**
--------------------------------------------------------------------------------------------
remote_request_callback
--------------------------------------------------------------------------------------------
*	This is the thread for the remote request task
*
* 	@\param		  none
*
* 	@\return		none
*
*/
void *remote_request_callback(void *);

int socket_creation_server(int);


#endif