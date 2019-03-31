#ifndef I2C_H_
#define I2C_H_

/**
 * @\file	i2c.h
 * @\author	Steve Antony X
 * @\brief	This files contains the declarations and header files for the I2C 
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "common.h"
/*****************************************************************
						Function Protoypes
*****************************************************************/
/**
--------------------------------------------------------------------------------------------
i2c_setup
--------------------------------------------------------------------------------------------
*	This function helps in setting up the i2c bus for the slave
*
* 	@\param		i2c file descriptor, bus number, slave address in hexa
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int i2c_setup(int * , int ,int );

#endif