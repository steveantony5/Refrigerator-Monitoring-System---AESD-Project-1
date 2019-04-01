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
#include "common.h"

/*****************************************************************
                        MACROS
*****************************************************************/
#define PORT_NO 6001
#define LOOP_BACK_ADDR "127.0.0.1"
#define MAX_BUFFER_SIZE 200


/*****************************************************************
                        Function Prototypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
socket_creation_client
--------------------------------------------------------------------------------------------
*	This helps in restarting the timer after expiration
*
* 	@\param			port number of server, IP address
*
* 	@\return		error status
*
*/int socket_creation_client(int port,char ip[20]);