/**
 * @\file	post.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the lux sensor 
 * @\date	03/30/2019
 *
 */

/*****************************************************************
					     Includes
*****************************************************************/
#include "POST.h"

/*****************************************************************
					Power On Self Test - POST
*****************************************************************/
int startup_test()
{
	int ret_val;

	/*Checks the temperature sensor power on check*/ 
	ret_val = temp_sensor_init();
	if(ret_val == ERROR)
	{
		perror("Satrup test temperature init failed");
		temp_dead_flag = 1;
	}

	ret_val = (int)temp_in_celcius();
	if(ret_val <-40 || ret_val > 128)
	{
		perror("Sartup temperature value test failed");
		temp_dead_flag = 1;
	}
	
	if(lux_sensor_setup() == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}

	/*Checks the light sensor power on check*/
	if(indication_register() == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}
	
	ret_val = (int)get_lux();
	if(ret_val == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}
	
	/*Checks if the threads are spawned properly*/
	if(!remote_socket_thread_creation)
	{
		perror("Sartup remote request thread creation test failed");
		remote_socket_dead_flag = 1;
	}
	
	if(!temperature_thread_creation)
	{
		perror("Sartup temperature thread creation test failed");
		temp_dead_flag = 1;
	}
	

	if(!lux_thread_creation)
	{
		perror("Sartup lux thread creation test failed");
		lux_dead_flag = 1;
	}
	
	if(!logger_thread_creation)
	{
		perror("Sartup logger thread creation test failed");
		logger_dead_flag = 1;
	}
	
	if(!lux_dead_flag)
	{
		start_lux_thread = 1;
	}	
	

	if(!temp_dead_flag)
	{
		start_temp_thread = 1;
	}
	
	/*returns error code for any failure in POST*/
	if(temp_dead_flag || lux_dead_flag || remote_socket_dead_flag || logger_dead_flag)
	{
		return ERROR;
	}
	

	return SUCCESS;
}
