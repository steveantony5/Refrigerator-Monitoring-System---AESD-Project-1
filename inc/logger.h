/**
 * @\file	logger.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the logger 
 * @\date	03/30/2019
 *
 */

#ifndef LOGGER_H
#define LOGGER_H


/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h> // mkdir
#include <mqueue.h>
#include <time.h>
#include <sys/syscall.h>
#include "POSIX_timer.h"
#include "common.h"


/*****************************************************************
						     Macros
*****************************************************************/
#define LOG_PATH "./log_folder/"

#define LOG_MESSAGE(file_name,...) { \
						  fprintf(file_ptr, ##__VA_ARGS__);\
						  fflush(file_ptr);\
						  }

#define SOURCE_ID(source_id) { \
						  sprintf(source_id,"[PID:%d] [TID:%lu] %s", getpid(), syscall(SYS_gettid), __func__);\
						}


#define QUEUE_NAME "/msg_queue"
#define MAX_BUFFER_SIZE	200


/*****************************************************************
						     Globals
*****************************************************************/
typedef struct
{
	char *file_name;
	char *file_path;	
}file_descriptors;

mqd_t msg_queue; // Message queue descriptor

char buffer[MAX_BUFFER_SIZE];
pthread_t logger_thread;
FILE *file_ptr;

extern char * log_t;


/*
--------------------------------------------------------------------------------------------
hanler_kill_log
--------------------------------------------------------------------------------------------
*	This is signal handler for logger thread
*
* 	@\param			signal number
*
* 	@\return		none
*
*/
void hanler_kill_log(int num);


/**
--------------------------------------------------------------------------------------------
time_stamp
--------------------------------------------------------------------------------------------
*	This function will format the timestamp
*
* 	@\param			
*
* 	@\return		timestamp as a string
*
*/
char *time_stamp();


/**
--------------------------------------------------------------------------------------------
logger_init
--------------------------------------------------------------------------------------------
*	This function will initialize the logger
*
* 	@\param			char *		filepath
*
* 	@\return		void
*
*/
void logger_init(char *);


/**
--------------------------------------------------------------------------------------------
logger_thread_callback
--------------------------------------------------------------------------------------------
*	This function is the thread callback function for the logger
*
* 	@\param			void *		filename and path
*
* 	@\return		void
*
*/
void *logger_thread_callback(void *);


#endif
