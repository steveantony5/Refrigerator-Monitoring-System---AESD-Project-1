#ifndef LUX_H_
#define LUX_H_


/**
 * @\file	lux.h
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
#include "common.h"

/*****************************************************************
						MACROS
*****************************************************************/
#define WRITE_COMMAND (0x80)
#define WRITE_COMMAND_WORD (0x20)


/*****************************************************************
				Function Prototypes
*****************************************************************/
/*
--------------------------------------------------------------------------------------------
byte_access_lux_register
--------------------------------------------------------------------------------------------
*	This function helps to write/read in the lux registers as byte
*
* 	@\param			i2c file descriptor\
*                   address of the register where it has to be read/written
*                   mode - read - 0/ write - 1/ command - 2(MACROS defined for these)
*                   register data pointer - where data to be stored for read mode
*											this data will be written in write mode
*                   flags - here any flag that has to be included in command registers can be passe
*                           eg: flag for clearing interrupts 
*                   
*
* 	@\return		error status
*
*/
int byte_access_lux_register(int , uint8_t ,int  , uint8_t *, uint8_t );

#endif