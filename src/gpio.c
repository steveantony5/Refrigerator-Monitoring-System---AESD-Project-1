/**
 * @\file	gpio.c
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the definitions for the gpio pins 
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include "gpio.h"


int led_on(int8_t pin)
{
	char gpio_path[MAX_BUFFER_SIZE];
	int8_t gpio_pin;

	gpio_pin = USR_LED_BASE + pin;

	/* Path to the specific USR LED file system */
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

	/* Path to the specific USR LED file system */
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

	/* Path to the specify USR LED as output */
	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/direction");
	file_ptr = fopen(gpio_path, "w+");

	fprintf(file_ptr,"out");
	fclose(file_ptr);

	return SUCCESS;	

}

int gpio_interrupt_init(int8_t gpio_pin)
{
	char gpio_path[MAX_BUFFER_SIZE];

	FILE *file_ptr = fopen("/sys/class/gpio/export", "w");
	
	fprintf(file_ptr,"%d", gpio_pin);
	fclose(file_ptr);

	/* Path to the specify GPIO as input */
	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/direction");
	file_ptr = fopen(gpio_path, "w+");

	fprintf(file_ptr,"in");
	fclose(file_ptr);

	memset(gpio_path,'\0',MAX_BUFFER_SIZE);
	sprintf(gpio_path,"%s%d%s","/sys/class/gpio/gpio",gpio_pin,"/edge");
	file_ptr = fopen(gpio_path, "w+");

	fprintf(file_ptr,"both");
	fclose(file_ptr);

	return SUCCESS;
}
