#include "temp.h"

int file_des_temp;

uint16_t default_configuration = 0x60A0;


int temp_sensor_init()
{
	return i2c_setup(&file_des_temp,2,TEMP_ADDR); 
}

int pointer_reg_write(pointer_reg reg)
{
	int8_t buffer = reg;
	int ret_val = write(file_des_temp, &buffer, sizeof(buffer));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	return 0;
}

int config_reg_read(uint16_t *configuration)
{
	pointer_reg_write(configReg);
	uint16_t config;

	uint8_t readBytes[2] = {0};

	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	config = (readBytes[0] << 8 | readBytes[1]);

	printf("CONFIG REG READ = %x\n", config);

	*configuration = config;

	return 0;

}

int config_reg_write_default()
{
	pointer_reg_write(configReg);

	uint8_t higherByte = default_configuration >> 8;
	uint8_t lowerByte = (uint8_t)default_configuration;

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	return 0;
}

int config_sd()
{
	pointer_reg_write(configReg);

	uint16_t writeConfig;

	config_reg_read(&writeConfig);

	writeConfig |= SD_MODE; 

	uint8_t higherByte = writeConfig >> 8;
	uint8_t lowerByte = (uint8_t)writeConfig;


	printf("writeConfig = %x\n", writeConfig);

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	return 0;
}

int config_sd_continuous()
{
	pointer_reg_write(configReg);

	uint16_t writeConfig;

	config_reg_read(&writeConfig);

	writeConfig &= ~(SD_MODE); 

	uint8_t higherByte = writeConfig >> 8;
	uint8_t lowerByte = (uint8_t)writeConfig;


	printf("writeConfig = %x\n", writeConfig);

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	return 0;
}


int config_read_conversion_rate()
{
	pointer_reg_write(configReg);

	uint8_t readBytes[2];

	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));

	if(ret_val == -1)
	{
		perror("Error on writing POINTER REGISTER");
		return -1;
	}

	printf("conversion rate prior1 = %x\n", readBytes[0]);
	printf("conversion rate prior 2 = %x\n", readBytes[1]);

	readBytes[1] &= 0xC0;
	readBytes[1] = readBytes[1] >> 6;

	printf("conversion rate = %x\n", readBytes[1]);

	return 0;
}


int tlow_reg_write(uint8_t temp_in_C)
{
	pointer_reg_write(tlowReg);

	uint8_t writeBytes[2] = {tlowReg, temp_in_C};
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	if(ret_val == -1)
	{
		perror("Error on writing TLOW REGISTER");
		return -1;
	}

	return 0;

}

int thigh_reg_write(uint8_t temp_in_C)
{
	pointer_reg_write(tlowReg);
	uint8_t writeBytes[2] = {thighReg, temp_in_C};
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	if(ret_val == -1)
	{
		perror("Error on writing TLOW REGISTER");
		return -1;
	}

	return 0;
}

int tlow_reg_read()
{
	uint8_t higherByte;
	uint8_t lowerByte;

	pointer_reg_write(tlowReg);

	uint16_t measured_tlow;

	int8_t readBytes[2] = {0};

	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	if(ret_val == -1)
	{
		perror("Error on reading TLOW REGISTER");
		return -1;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

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
	if(ret_val == -1)
	{
		perror("Error on reading TLOW REGISTER");
		return -1;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

	measured_thigh = (higherByte << 8) | lowerByte;
	measured_thigh = measured_thigh >> 4;

	return measured_thigh;
}

int temp_read()
{
	uint8_t higherByte;
	uint8_t lowerByte;
	int measured_temperature;

	int8_t readBytes[2] = {0};

	pointer_reg_write(tempReg);

	//pointer reg write for temperature conversion
	
	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));

	if(ret_val == -1)
	{
		perror("Error on reading TEMPERATURE REGISTER");
		return -1;
	}

	higherByte = readBytes[0];
	lowerByte = readBytes[1];

	measured_temperature = (higherByte << 8) | lowerByte;
	measured_temperature = measured_temperature >> 4;

	return measured_temperature;
}

float temp_in_celcius()
{
	return (temp_read() * 0.0625);
}
