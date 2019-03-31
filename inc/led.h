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
#include "common.h"


/*****************************************************************
						Function prototypes
*****************************************************************/
int led_on();

int led_off();

int gpio_pin_init();

#endif