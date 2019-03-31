

#ifndef POSIX_Timer_H_
#define POSIX_Timer_H_

/**
 * @\file	POSIX_timer.h
 * @\author	Steve Antony X
 * @\brief	This files contains the declarations and header files for POSIX timer modules
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "common.h"
/*****************************************************************
						Function Protypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
kick_timer
--------------------------------------------------------------------------------------------
*	This helps in restarting the timer after expiration
*
* 	@\param			timer descriptor, timer expiration time in ns
*
* 	@\return		error status
*
*/
int kick_timer(timer_t, int);

/*
--------------------------------------------------------------------------------------------
setup_timer_POSIX
--------------------------------------------------------------------------------------------
*	This helps in creating the timer
*
* 	@\param			timer descriptor, timer handler function
*
* 	@\return		error status
*
*/
int setup_timer_POSIX(timer_t *,void (*handler)(union sigval));

/*
--------------------------------------------------------------------------------------------
stop_timer
--------------------------------------------------------------------------------------------
*	This helps in deleting the timer
*
* 	@\param			timer descriptor
*
* 	@\return		error status
*
*/
int stop_timer(timer_t);

/*
--------------------------------------------------------------------------------------------
temp_timer_handler
--------------------------------------------------------------------------------------------
*	This is the timer handler for temperature timer
*
* 	@\param			dummy
*
* 	@\return		error status
*
*/
void temp_timer_handler(union sigval);

/*
--------------------------------------------------------------------------------------------
lux_timer_handler
--------------------------------------------------------------------------------------------
*	This is the timer handler for lux timer
*
* 	@\param			dummy
*
* 	@\return		error status
*
*/
void lux_timer_handler(union sigval);

/*
--------------------------------------------------------------------------------------------
log_timer_handler
--------------------------------------------------------------------------------------------
*	This is the timer handler for log timer
*
* 	@\param			dummy
*
* 	@\return		error status
*
*/
void log_timer_handler(union sigval);

/*****************************************************************
						Globals
*****************************************************************/
extern timer_t timer_id_temp, timer_id_lux, timer_id_heartbeat, timer_id_log;
extern int FLAG_READ_TEMP;
extern int FLAG_READ_LUX;
extern int FLAG_LOG;
/*****************************************************************
						MACROS
*****************************************************************/
#define Delay_NS (200000000)//200ms


#endif /* POSIX_Timer_H_ */