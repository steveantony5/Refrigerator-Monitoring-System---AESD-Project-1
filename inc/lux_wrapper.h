#ifndef LUX_WRAPPER_H_
#define LUX_WRAPPER_H_


/**
 * @\file	lux_wrapper.h
 * @\author	Steve Antony X
 * @\brief	This files contains the declarations and header files for the lux sensor 
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "i2c.h"
#include "logger.h"
#include "lux.h"
#include "led.h"

/*****************************************************************
						MACROS
*****************************************************************/
#define CONTROL_REGISTER (0X00)
#define TIMING_REGISTER (0X01)
#define THRESHLOWLOW (0x02)
#define THRESHLOWHIGH (0x03)
#define THRESHHIGHLOW (0x04)
#define THRESHHIGHHIGH (0x05)
#define INTERRUPT (0x06)
#define INDICATION_REGISTER (0x0A)
#define DATA0LOW_REGISTER (0X0C)
#define DATA0HIGH_REGISTER (0X0D)
#define DATA1LOW_REGISTER (0X0E)
#define DATA1HIGH_REGISTER (0X0F)

#define CLEAR_PENDING_INTERUPTS (0x40)

#define THRESHOLD_FOR_STATE_CHANGE (70)

#define WRITE (1)
#define COMMAND (2)
#define READ (0)
#define NONE (0)
/*****************************************************************
						Global declaration
*****************************************************************/
extern pthread_mutex_t lock_res;

/*****************************************************************
						Function Protypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
lux_sensor_setup
--------------------------------------------------------------------------------------------
*	This is a wrapper function to set up the lux sensor and its registers
*
* 	@\param			none
*
* 	@\return		error status
*
*/
int lux_sensor_setup();

/*
--------------------------------------------------------------------------------------------
read_channel_0
--------------------------------------------------------------------------------------------
*	This is a wrapper function that reads CH0 of the lux sensor
*
* 	@\param			none
*
* 	@\return		error status
*
*/
int read_channel_0();

/*
--------------------------------------------------------------------------------------------
read_channel_1
--------------------------------------------------------------------------------------------
*	This is a wrapper function that reads CH1 of the lux sensor
*
* 	@\param			none
*
* 	@\return		error status
*
*/
int read_channel_1();

/*
--------------------------------------------------------------------------------------------
lux_measurement
--------------------------------------------------------------------------------------------
*	This function measures lux value based on CH0 and CH1
*
* 	@\param			Ch0, CH1
*
* 	@\return		lux value
*
*/
float lux_measurement(float , float );

/*
--------------------------------------------------------------------------------------------
has_state_transition_occurred
--------------------------------------------------------------------------------------------
*	This function identifies whether a transition has occured from bright to dark or dark to bright
*
* 	@\param			lux
*
* 	@\return		none
*
*/
void has_state_transition_occurred(float );

/*
--------------------------------------------------------------------------------------------
get_lux
--------------------------------------------------------------------------------------------
*	This is a wrapper for lux sensor measurement for remote request task
*   This includes read_channel_0(), read_channel_1(), lux_measurement(float , float )
*
* 	@\param			none
*
* 	@\return		lux value
*
*/
float get_lux();

/*
--------------------------------------------------------------------------------------------
get_current_state_fridge
--------------------------------------------------------------------------------------------
*	This gets the current staus of the fridge door whether it is opened or closed
*
* 	@\param			lux value
*
* 	@\return		door status 
*
*/
enum Status get_current_state_fridge(float);

/*
--------------------------------------------------------------------------------------------
indication_register
--------------------------------------------------------------------------------------------
*	This is a wrapper function that gets the part number and revision number of the sensor 
*   which is used in start up test
*
* 	@\param			lux value
*
* 	@\return		error code 
*
*/
int indication_register();

#endif