#include "lux.h"

int file_des_lux;
uint8_t register_data;
uint8_t MSB_0;
uint8_t LSB_0;

uint8_t MSB_1;
uint8_t LSB_1;

uint16_t CH0;
uint16_t CH1;




int lux_sensor_setup()
{

	//command to write on control register
	register_data = WRITE_COMMAND | CONTROL_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//value for the control register
	register_data = 0x03;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//command to write on timing register
	register_data = WRITE_COMMAND | TIMING_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//value for the control register
	register_data = 0x11;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the timing register\n");
		return -1;
	}

	return 0;

}

int read_channel_0()
{
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA0LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des_lux, &LSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0LOW_REGISTER register\n");
		return -1;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA0HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des_lux, &MSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0HIGH_REGISTER register\n");
		return -1;
	}

	CH0 = MSB_0 < 8;
	CH0 |= LSB_0;

	return 0;


}

int read_channel_1()
{
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA1LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des_lux, &LSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1LOW_REGISTER register\n");
		return -1;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA1HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des_lux, &MSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1HIGH_REGISTER register\n");
		return -1;
	}

	CH1 = MSB_1 < 8;
	CH1 |= LSB_1;

	return 0;

 
}


float lux_measurement(float CH0, float CH1)
{

	float ratio = (CH1 / CH0);

	//0 < CH1/CH0 ≤ 0.50 Sensor Lux = (0.0304 x CH0) – (0.062 x CH0 x ((CH1/CH0)1.4))

	if((ratio <=0.5)&& (ratio > 0))
		return ((0.0304 * CH0) - (0.062 * CH0 * (powf(ratio, 1.4))));

	//0.50 < CH1/CH0 ≤ 0.61 Sensor Lux = (0.0224 x CH0) – (0.031 x CH1)

	else if((ratio  > 0.5)&& (ratio <= 0.61))
		return ((0.0224 * CH0) - (0.031 * CH1));

    //0.61 < CH1/CH0 ≤ 0.80 Sensor Lux = (0.0128 x CH0) – (0.0153 x CH1)
	else if((ratio  > 0.61)&& (ratio <= 0.8))
		return (0.0128 * CH0) - (0.0153 * CH1);

	//0.80 < CH1/CH0 ≤ 1.30 Sensor Lux = (0.00146 x CH0) – (0.00112 x CH1)
	else if((ratio  > 0.80)&& (ratio <= 1.30))
		return (0.00146 * CH0) - (0.00112 * CH1);

	//CH1/CH0>1.30 Sensor Lux = 0
	else if(ratio  > 1.30)
		return 0;

	else
	{
		printf("Invalid\n");
		return -1;
	}


}