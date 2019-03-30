/**
 * @\file	temp.c
 * @\author	Sanju Prakash Kannioth
 * @\brief	This files contains the definitions of functions for the tmp102 temperature sensor
 * @\date	03/29/2019
 *
 */

#include "temp.h"

int file_des_temp;

uint16_t default_configuration = 0x60A0; // Global variable to store the default temperature configurations

int config_reg_write_update(uint16_t bit_mask, bool clear)
{
	pointer_reg_write(configReg);

	uint16_t writeConfig;

	config_reg_read(&writeConfig);

	/* Check if bit needs to be cleared or set */
	if(!clear)
		writeConfig |= bit_mask; 
	else
		writeConfig &= ~(bit_mask);

	/*Formatting bytes for writing */
	uint8_t higherByte = writeConfig >> 8;
	uint8_t lowerByte = (uint8_t)writeConfig;


	printf("writeConfig = %x\n", writeConfig);

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));

	if(ret_val == ERROR)
	{
		perror("Error on writing CONFIGURATION REGISTER");
		return ERROR;
	}

	return SUCCESS;
}

int config_reg_read_update(uint8_t bit_mask, uint8_t byte, uint8_t shift)
{
	pointer_reg_write(configReg);

	uint8_t readBytes[2];

	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));

	if(ret_val == ERROR)
	{
		perror("Error on reading CONFIGURATION REGISTER");
		return ERROR;
	}

	printf("conversion rate prior1 = %x\n", readBytes[0]);
	printf("conversion rate prior 2 = %x\n\n", readBytes[1]);

	/*Reads the required bits from the byte*/
	readBytes[byte] &= bit_mask;
	readBytes[byte] = readBytes[byte] >> shift;

	printf("bits = %x\n", readBytes[byte]);

	return readBytes[byte];
}

int temp_sensor_init()
{
	return i2c_setup(&file_des_temp,2,TEMP_ADDR); 
}

int pointer_reg_write(pointer_reg reg)
{
	int8_t buffer = reg;

	int ret_val = write(file_des_temp, &buffer, sizeof(buffer));

	if(ret_val == ERROR)
	{
		perror("Error on writing POINTER REGISTER");
		return ERROR;
	}

	return SUCCESS;
}

int config_reg_read(uint16_t *configuration)
{
	pointer_reg_write(configReg);
	uint16_t config;

	uint8_t readBytes[2] = {0};

	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));

	if(ret_val == ERROR)
	{
		perror("Error on reading CONFIGURATION REGISTER");
		return ERROR;
	}

	config = (readBytes[0] << 8 | readBytes[1]);

	printf("CONFIG REG READ = %x\n", config);

	*configuration = config;

	return SUCCESS;

}


int config_reg_write_default()
{
	pointer_reg_write(configReg);

	uint8_t higherByte = default_configuration >> 8;
	uint8_t lowerByte = (uint8_t)default_configuration;

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));

	if(ret_val == ERROR)
	{
		perror("Error on writing CONFIGURATION REGISTER");
		return ERROR;
	}

	return SUCCESS;
}

int config_sd()
{
	return config_reg_write_update(SD_MODE, 0);	
}

int config_sd_continuous()
{
	return config_reg_write_update(SD_MODE, 1);	
}

int config_fault_bits()
{
	return config_reg_write_update(2<<11, 0);
}

int config_em_normal()
{
	return config_reg_write_update(EM_MODE, 1);
}

int config_em_extended()
{
	return config_reg_write_update(EM_MODE, 0);
}


int config_read_conversion_rate()
{
	return config_reg_read_update(READ_CONVERSION, 1, 6);

}

int config_read_fault_bits()
{
	return config_reg_read_update(READ_FAULT_BITS, 0, 3);
}

int config_read_em()
{
	return config_reg_read_update(READ_EM_MODE, 1, 4);
}

int config_conversion_rate_0_25HZ()
{
	config_reg_write_update(CR0, 1);
	config_reg_write_update(CR1, 1);

	return SUCCESS;

}

int config_conversion_rate_1HZ()
{
	config_reg_write_update(CR0, 0);
	config_reg_write_update(CR1, 1);

	return SUCCESS;
}

int config_conversion_rate_4HZ()
{
	config_reg_write_update(CR0, 1);
	config_reg_write_update(CR1, 0);

	return SUCCESS;
}

int config_conversion_rate_8HZ()
{
	config_reg_write_update(CR0, 0);
	config_reg_write_update(CR1, 0);

	return SUCCESS;
}

int config_read_alert()
{
	return config_reg_read_update(READ_ALERT, 1, 5); 
}
int tlow_reg_write(uint8_t temp_in_C)
{
	pointer_reg_write(tlowReg);

	uint8_t writeBytes[2] = {tlowReg, temp_in_C};
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	if(ret_val == ERROR)
	{
		perror("Error on writing TLOW REGISTER");
		return ERROR;
	}

	return SUCCESS;

}

int thigh_reg_write(uint8_t temp_in_C)
{
	pointer_reg_write(tlowReg);
	uint8_t writeBytes[2] = {thighReg, temp_in_C};
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	if(ret_val == ERROR)
	{
		perror("Error on writing THIGH REGISTER");
		return ERROR;
	}

	return SUCCESS;
}

int tlow_reg_read()
{
	uint8_t higherByte;
	uint8_t lowerByte;

	pointer_reg_write(tlowReg);

	uint16_t measured_tlow;

	int8_t readBytes[2] = {0};

	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	if(ret_val == ERROR)
	{
		perror("Error on reading TLOW REGISTER");
		return ERROR;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

	/* Formatting the temperature value received according to the datasheet */
	measured_tlow = (higherByte << 8) | lowerByte;
	measured_tlow = measured_tlow >> 4;

	return measured_tlow;


}

int thigh_reg_read()
{
	uint8_t higherByte;
	uint8_t lowerByte;

	pointer_reg_write(thighReg);

	uint16_t measured_thigh;

	int8_t readBytes[2] = {0};

	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	if(ret_val == ERROR)
	{
		perror("Error on reading TLOW REGISTER");
		return ERROR;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

	/* Formatting the temperature value received according to the datasheet */
	measured_thigh = (higherByte << 8) | lowerByte;
	measured_thigh = measured_thigh >> 4;

	return measured_thigh;
}

int temp_read()
{
	uint8_t higherByte;
	uint8_t lowerByte;
	int16_t measured_temperature;

	int8_t readBytes[2] = {0};

	pointer_reg_write(tempReg);

	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));

	if(ret_val == ERROR)
	{
		perror("Error on reading TEMPERATURE REGISTER");
		return ERROR;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

	/* Formatting the temperature value received according to the datasheet */
	measured_temperature = (higherByte << 8) | lowerByte;
	measured_temperature = measured_temperature >> 4;

	/* Check if MSB is set */
	if(measured_temperature & 0x800)
	{
		measured_temperature = (~(measured_temperature) + 1) & 0xFFF;
		measured_temperature = -1 * measured_temperature;
	}


	return measured_temperature;
}

float temp_in_celcius()
{
	return (temp_read() * 0.0625);
}

float temp_in_kelvin()
{
	return ((temp_read() * 0.0625) + 273.15);
}

float temp_in_fahrenheit()
{
	return (((temp_read() * 0.0625) * (9/5)) + 32);
}