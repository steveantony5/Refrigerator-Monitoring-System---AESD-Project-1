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


int led_on(int8_t pin)
{
	char gpio_path[MAX_BUFFER_SIZE];
	int8_t gpio_pin;

	gpio_pin = USR_LED_BASE + pin;

	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/value");

	FILE *file_ptr = fopen(gpio_path, "w");
	
	fputc('1', file_ptr);
	fclose(file_ptr);	

	return SUCCESS;
}

int led_off(int8_t pin)
{
	char gpio_path[MAX_BUFFER_SIZE];
	int8_t gpio_pin;

	gpio_pin = USR_LED_BASE + pin;

	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/value");

	FILE *file_ptr = fopen(gpio_path, "w");
	
	fputc('0', file_ptr);
	fclose(file_ptr);	

	return SUCCESS;
}

int gpio_pin_init(int8_t pin)
{
	char gpio_path[MAX_BUFFER_SIZE];
	int8_t gpio_pin;

	gpio_pin = USR_LED_BASE + pin;

	FILE *file_ptr = fopen("/sys/class/gpio/export", "w");
	
	fprintf(file_ptr,"%d", gpio_pin);
	fclose(file_ptr);

	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/direction");
	file_ptr = fopen(gpio_path, "w+");

	fprintf(file_ptr,"out");
	fclose(file_ptr);

	return SUCCESS;	

}
