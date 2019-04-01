#ifndef POST_H
#define POST_H

/**
 * @\file	lux_wrapper.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the Power On Self Test 
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include "logger.h"
#include "lux_wrapper.h"
#include "temp.h"
#include "i2c.h"
#include "common.h"

/*****************************************************************
						Global decalarations
*****************************************************************/
extern int logger_thread_creation;
extern int remote_socket_thread_creation;
extern int temperature_thread_creation;
extern int lux_thread_creation;

extern int temp_dead_flag;
extern int lux_dead_flag;
extern int remote_socket_dead_flag;
extern int logger_dead_flag;

extern volatile int start_temp_thread;
extern volatile int start_lux_thread;

/*****************************************************************
					Function Prototypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
startup_test
--------------------------------------------------------------------------------------------
*	This contains the power on self test for temperature sensor and light sensor
*
* 	@\param			signal number
*
* 	@\return		none
*
*/
int startup_test();

#endif