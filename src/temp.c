#include "temp.h"

int file_des_temp;


int temp_sensor_init()
{
	return i2c_setup(file_des_temp,2,TEMP_ADDR); 
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

/*
int tlow_reg_write()
{
	pointer_reg_write(tlowReg);

	int8_t readBytes = 160;
	int ret_val = write(file_des, &readBytes,sizeof(readBytes));
	if(ret_val == -1)
	{
		perror("Error on writing TLOW REGISTER");
		return -1;
	}

	return 0;

}*/

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

/*
uint16_t tlow_reg_read()
{

	uint16_t buff;
	pointer_reg_write(tlowReg);
	int ret =  read(file_des,&buff,sizeof(buff));
	if (ret < 0)
		perror("Read tlow reg failed");
	
	return buff;
}*/

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

/*int main()
{
	temp_sensor_init();

	//tlow_reg_write(160);

	while(1)
	{
		sleep(1);
		printf("\nTemperature in celcius = %f", temp_read() * 0.0625);
		printf("\nT-high in celcius = %f", thigh_reg_read() * 0.0625);
		printf("\nT-low in celcius = %f\n", tlow_reg_read() * 0.0625);
	}
}*/
