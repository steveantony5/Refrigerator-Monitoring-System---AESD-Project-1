/**
 * @\file	gpio.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the gpio pins 
 * @\date	03/30/2019
 *
 */

#ifndef GPIO_H
#define GPIO_H


/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

/*****************************************************************
						     Macros
*****************************************************************/

#define USR_LED_BASE 53 // Pin for 1st USR LED on BBG
#define MAX_BUFFER_SIZE 200

/*****************************************************************
						Function prototypes
*****************************************************************/


/**
--------------------------------------------------------------------------------------------
led_on
--------------------------------------------------------------------------------------------
*	This function will turn the user specified USR LED ON
*
* 	@\param			int8_t 		USR LED number
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int led_on(int8_t);


/**
--------------------------------------------------------------------------------------------
led_off
--------------------------------------------------------------------------------------------
*	This function will turn the user specified USR LED OFF
*
* 	@\param			int8_t 		USR LED number
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int led_off(int8_t);


/**
--------------------------------------------------------------------------------------------
gpio_pin_init
--------------------------------------------------------------------------------------------
*	This function will initialize the user specified USR LED
*
* 	@\param			int8_t 		USR LED number
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int gpio_pin_init(int8_t);

#endif