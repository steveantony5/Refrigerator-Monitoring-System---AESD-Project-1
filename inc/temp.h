/**
 * @\file	temp.h
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the declarations and header files for the tmp102 temperature sensor
 * @\date	03/29/2019
 *
 */

#ifndef TEMP_H_
#define TEMP_H_

/**
 * INCLUDES
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "i2c.h"

/**
 * MACROS
 *
 */
#define TEMP_ADDR 0x48 // Slave address of temperature sensor

#define SD_MODE (1 << 8) // Bit mask for shutdown configuration
#define EM_MODE (1 << 4) // Bit mask for extended mode configuration
#define CR0		(1 << 6) // Bit mask for conversion mode configuration 1st bit
#define CR1		(1 << 7) // Bit mask for conversion mode configuration 2nd bit


#define READ_CONVERSION (0xC0) 
#define READ_FAULT_BITS (0x18)
#define READ_EM_MODE (0x10)


/**
 * ENUMS
 *
 */
typedef enum pointer_reg{
	tempReg,
	configReg,
	tlowReg,
	thighReg
}pointer_reg;


/**
--------------------------------------------------------------------------------------------
temp_sensor_init
--------------------------------------------------------------------------------------------
*	This function will initialize the temperature sensor
*
* 	@\param			
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int temp_sensor_init();

/**
--------------------------------------------------------------------------------------------
pointer_reg_write
--------------------------------------------------------------------------------------------
*	This function will point to the register that needs to be writen/read
*
* 	@\param			 
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int pointer_reg_write(pointer_reg reg);

/**
--------------------------------------------------------------------------------------------
tlow_reg_read
--------------------------------------------------------------------------------------------
*	This function will read the tlow register
*
* 	@\param			
*
* 	@\return		tlow configured temperature
*
*/
int tlow_reg_read();


/**
--------------------------------------------------------------------------------------------
thigh_reg_read
--------------------------------------------------------------------------------------------
*	This function will read the thigh register
*
* 	@\param			
*
* 	@\return		thigh configured temperature
*
*/
int thigh_reg_read();


/**
--------------------------------------------------------------------------------------------
temp_read
--------------------------------------------------------------------------------------------
*	This function will read the temperature data register
*
* 	@\param			
*
* 	@\return		raw temperature value from sensor
*
*/
int temp_read();


/**
--------------------------------------------------------------------------------------------
temp_in_celcius
--------------------------------------------------------------------------------------------
*	This function will return the temperature in celcius
*
* 	@\param			
*
* 	@\return		temperature in celcius
*
*/
float temp_in_celcius();


/**
--------------------------------------------------------------------------------------------
temp_in_kelvin
--------------------------------------------------------------------------------------------
*	This function will return the temperature in kelvin
*
* 	@\param			
*
* 	@\return		temperature in kelvin
*
*/
float temp_in_kelvin();


/**
--------------------------------------------------------------------------------------------
temp_in_fahrenheit
--------------------------------------------------------------------------------------------
*	This function will return the temperature in fahrenheit
*
* 	@\param			
*
* 	@\return		temperature in fahrenheit
*
*/
float temp_in_fahrenheit()

/**
--------------------------------------------------------------------------------------------
tlow_reg_write
--------------------------------------------------------------------------------------------
*	This function will write the tlow register
*
* 	@\param			temp_in_C  temperature in celcius
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int tlow_reg_write(uint8_t);


/**
--------------------------------------------------------------------------------------------
thigh_reg_write
--------------------------------------------------------------------------------------------
*	This function will write the thigh register
*
* 	@\param			temp_in_C  temperature in celcius
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int thigh_reg_write(uint8_t);


/**
--------------------------------------------------------------------------------------------
config_reg_read
--------------------------------------------------------------------------------------------
*	This function will read the configuration register
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_reg_read();


/**
--------------------------------------------------------------------------------------------
config_reg_write_default
--------------------------------------------------------------------------------------------
*	This function will write the default values to the configuration register
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_reg_write_default();


/**
--------------------------------------------------------------------------------------------
config_sd_continuous
--------------------------------------------------------------------------------------------
*	This function will write 0 to the shutdown bit in the configuration register. 
*	Allows conitunous conversion.
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_sd_continuous();


/**
--------------------------------------------------------------------------------------------
config_sd
--------------------------------------------------------------------------------------------
*	This function will write 1 to the shutdown bit in the configuration register.
*	Disables continuous conversion.
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_sd();


/**
--------------------------------------------------------------------------------------------
config_fault_bits
--------------------------------------------------------------------------------------------
*	This function will write to the fault bits in the configuration register.
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_fault_bits();

/**
--------------------------------------------------------------------------------------------
config_em_normal
--------------------------------------------------------------------------------------------
*	This function will write to the em bit in the configuration register.
*	Enables 12 bit mode.
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_em_normal();


/**
--------------------------------------------------------------------------------------------
config_em_extended
--------------------------------------------------------------------------------------------
*	This function will write to the em bit in the configuration register.
*	Enables 13 bit mode.
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_em_extended();

/**
--------------------------------------------------------------------------------------------
config_conversion_rate_0_25HZ
--------------------------------------------------------------------------------------------
*	This function will write to the CR bits in the configuration register.
*	Sets the conversion rate to 0.25hz
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_conversion_rate_0_25HZ();


/**
--------------------------------------------------------------------------------------------
config_conversion_rate_1HZ
--------------------------------------------------------------------------------------------
*	This function will write to the CR bits in the configuration register.
*	Sets the conversion rate to 1hz
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_conversion_rate_1HZ();


/**
--------------------------------------------------------------------------------------------
config_conversion_rate_4HZ
--------------------------------------------------------------------------------------------
*	This function will write to the CR bits in the configuration register.
*	Sets the conversion rate to 4hz
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_conversion_rate_4HZ();


/**
--------------------------------------------------------------------------------------------
config_conversion_rate_8HZ
--------------------------------------------------------------------------------------------
*	This function will write to the CR bits in the configuration register.
*	Sets the conversion rate to 8hz
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_conversion_rate_8HZ();


/**
--------------------------------------------------------------------------------------------
config_read_em
--------------------------------------------------------------------------------------------
*	This function will read the mode of operation from the configuration register
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_read_em();

/**
--------------------------------------------------------------------------------------------
config_read_fault_bits
--------------------------------------------------------------------------------------------
*	This function will read the fault queue from the configuration register
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_read_fault_bits();

/**
--------------------------------------------------------------------------------------------
config_read_conversion_rate
--------------------------------------------------------------------------------------------
*	This function will read the conversion rate from the configuration register
*
* 	@\param
*
* 	@\return		On success it returns 0, 
* 					On failure it returns -1
*
*/
int config_read_conversion_rate();

#endif