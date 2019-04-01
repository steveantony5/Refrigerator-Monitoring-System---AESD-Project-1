#ifndef LED_H
#define LED_H


/**
 * @\file	led.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the led 
 * @\date	03/30/2019
 *
 */
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

#define USR_LED_BASE 53
#define MAX_BUFFER_SIZE 200

/*****************************************************************
						Function prototypes
*****************************************************************/
int led_on(int8_t);

int led_off(int8_t);

int gpio_pin_init(int8_t);

#endif