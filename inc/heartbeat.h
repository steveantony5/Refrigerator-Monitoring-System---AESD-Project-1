#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_
/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <pthread.h>
#include "POSIX_timer.h"
#include <sys/types.h>
#include "logger.h"
#include "remote_request.h"

/*****************************************************************
						Function Protypes
*****************************************************************/
void beat_timer_handler(union sigval );
void *temperature_task();
void *lux_task();

/*****************************************************************
						Globals
*****************************************************************/
// FIFO file path 
 extern char * Temp;
 extern char * Lux ;
 extern char * log_t;
/*****************************************************************
						MACROS
*****************************************************************/
#define HEART_BEAT_CHECK_PERIOD (900000000)//200ms


#endif /* HEARTBEAT_H_ */