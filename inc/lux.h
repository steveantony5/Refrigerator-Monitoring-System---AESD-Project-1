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

#define WRITE_COMMAND (0x80)
#define WRITE_COMMAND_WORD (0xA0)



int byte_access_lux_register(int , uint8_t ,int  , uint8_t *, uint8_t );
int word_access_lux_register(int , uint8_t ,int  ,uint16_t *, uint8_t );

#endif