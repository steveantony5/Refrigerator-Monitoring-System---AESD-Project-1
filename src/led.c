/**
 * @\file	led.c
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the definitions for the led 
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include "led.h"


int led_on()
{
	FILE *file_ptr = fopen("/sys/class/gpio/gpio53/value", "w");
	
	fputc('0', file_ptr);
	fclose(file_ptr);	

	return SUCCESS;
}

int led_off()
{
	FILE *file_ptr = fopen("/sys/class/gpio/gpio53/value", "w");
	
	fputc('1', file_ptr);
	fclose(file_ptr);	

	return SUCCESS;
}

int gpio_pin_init()
{
	FILE *file_ptr = fopen("/sys/class/gpio/export", "w");
	
	fprintf(file_ptr,"%d",53);
	fclose(file_ptr);

	file_ptr = fopen("/sys/class/gpio/gpio53/direction", "w+");

	fprintf(file_ptr,"out");
	fclose(file_ptr);

	return SUCCESS;	

}
