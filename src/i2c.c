/**
 * @\file	i2c.c
 * @\author	Steve Antony X
 * @\brief	This files contains the function definitions for I2C 
 * @\date	03/30/2019
 *
 */

/*****************************************************************
						Includes
*****************************************************************/
#include "i2c.h"

/*****************************************************************
						Global definition
*****************************************************************/
char bus[15];

/*****************************************************************
						setting up i2c driver
*****************************************************************/
int i2c_setup(int *desp, int bus_no,int address)
{
	memset(bus,0,sizeof(bus));

	sprintf(bus,"/dev/i2c-%d",bus_no);

	if((*desp = open(bus, O_RDWR)) < 0)
	{
		perror("Error on opening the bus\n");
		return ERROR;
	}

	if ((ioctl(*desp, I2C_SLAVE, address)) < 0)
	{
		perror("Error on input output control of bus\n");
		return ERROR;
	}

	return SUCCESS;

}