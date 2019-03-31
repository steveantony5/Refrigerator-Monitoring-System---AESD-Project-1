#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

/**
 * @\file	heartbeat.h
 * @\author	Steve Antony X
 * @\brief	This files contains the declarations and header files for the heartbeat and spawing of threads
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/

#include "POSIX_timer.h"
#include "logger.h"
#include "lux_wrapper.h"
#include "temp.h"
#include "i2c.h"
#include "led.h"
#include "remote_request.h"
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
/*****************************************************************
						Function Protypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
beat_timer_handler
--------------------------------------------------------------------------------------------
*	This function is the timer handler for heart beat timer
*
* 	@\param			signal value ( dummy)
*
* 	@\return		none
*
*/
void beat_timer_handler(union sigval );

/*
--------------------------------------------------------------------------------------------
temperature_task
--------------------------------------------------------------------------------------------
*	This is the temperature thread which will be spawned by the main task
*
* 	@\param			none
*
* 	@\return		none
*
*/
void *temperature_task();

/*
--------------------------------------------------------------------------------------------
lux_task
--------------------------------------------------------------------------------------------
*	This is the lux thread which will be spawned by the main task
*
* 	@\param			none
*
* 	@\return		none
*
*/
void *lux_task();

/*
--------------------------------------------------------------------------------------------
hanler_kill_temp
--------------------------------------------------------------------------------------------
*	This is signal handler for temperature thread
*
* 	@\param			signal number
*
* 	@\return		none
*
*/
void hanler_kill_temp(int num);

/*
--------------------------------------------------------------------------------------------
hanler_kill_lux
--------------------------------------------------------------------------------------------
*	This is signal handler for lux thread
*
* 	@\param			signal number
*
* 	@\return		none
*
*/
void hanler_kill_lux(int num);

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

/*
--------------------------------------------------------------------------------------------
hanler_kill_main
--------------------------------------------------------------------------------------------
*	This is signal handler for main thread
*
* 	@\param			signal number
*
* 	@\return		none
*
*/
void hanler_kill_main(int num);

/*****************************************************************
						Global declarations
*****************************************************************/
/* FIFO file path */
extern char * Temp;
extern char * Lux ;
extern char * log_t;

/*for lux measurement*/
extern uint16_t CH0;
extern uint16_t CH1;
extern int file_des_lux;

/*logger timer handler*/
extern timer_t timer_id_log;

/*Logger pipe descriptor*/
extern  int fd3_w;
/*****************************************************************
						MACROS
*****************************************************************/
#define HEART_BEAT_CHECK_PERIOD (900000000)//0.9 s


#endif /* HEARTBEAT_H_ */