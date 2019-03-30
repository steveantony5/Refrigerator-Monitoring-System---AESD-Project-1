#include "lux.h"

int file_des_lux;
uint8_t register_data;
uint16_t register_data_word;

uint16_t MSB_0;
uint16_t LSB_0;

uint16_t MSB_1;
uint16_t LSB_1;

uint16_t CH0;
uint16_t CH1;




int lux_sensor_setup()
{

	pthread_mutex_lock(&lock_res);
	if((i2c_setup(&file_des_lux,2,0x39)) == ERROR)
	{
		perror("Error on i2c bus set up for lux sensor");
		return ERROR;
	}

	//command to write on control register
	register_data = WRITE_COMMAND | CONTROL_REGISTER;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	//value for the control register
	register_data = 0x03;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	//command to write on timing register
	register_data = WRITE_COMMAND | TIMING_REGISTER;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	//value for the control registe16
	register_data = 0x12;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the timing register\n");
		return ERROR;
	}

	// //command to write on control register as a word for low threshold register 
	// register_data = WRITE_COMMAND_WORD | THRESHLOWLOW;

	// if (write(file_des_lux, &register_data, 1) == -1)
	// {
	// 	perror("Error on writing the control register\n");
	// 	return ERROR;
	// }

	// //value for the control registe16
	// register_data_word = 0x;

	// if (write(file_des_lux, &register_data_word, 2) == -1)
	// {
	// 	perror("Error on writing the low threshold register\n");
	// 	return ERROR;
	// }

	//command to write on control register as a word for high threshold register 
	register_data = WRITE_COMMAND_WORD | THRESHHIGHLOW;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	//value for the control registe16
	register_data_word = 0x03E8; //set to 1000

	if (write(file_des_lux, &register_data_word, 2) == ERROR)
	{
		perror("Error on writing the high threshold register\n");
		return ERROR;
	}

	//command to write on control register  for Interupt register 
	register_data = WRITE_COMMAND | INTERRUPT;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	//value for the INTERRUPT register
	register_data = 0x12; 

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the interrupt register\n");
		return ERROR;
	}

	pthread_mutex_unlock(&lock_res);

	return SUCCESS;

}


int indication_register()
{
	pthread_mutex_lock(&lock_res);
	//command to write on control register  for Interupt register 
	register_data = WRITE_COMMAND | INDICATION_REGISTER;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	if (read(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the interrupt register\n");
		return ERROR;
	}

	pthread_mutex_unlock(&lock_res);

	uint8_t part_no = register_data & (0xF0);
	part_no = part_no >> 4;
	printf("Part numer of Lux sensor %d\n",part_no);

	uint8_t rev_no = register_data & (0x0F);
	printf("Revision numer of Lux sensor %d\n",rev_no);


	memset(buffer,0,MAX_BUFFER_SIZE);
	sprintf(buffer,"LUX sensor\nPNO: %d\nRev no %d\n\n",part_no,rev_no);
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


	return SUCCESS;
}

int read_channel_0()
{

	pthread_mutex_lock(&lock_res);
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA0LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}


	if (read(file_des_lux, &LSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0LOW_REGISTER register\n");
		return ERROR;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA0HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}


	if (read(file_des_lux, &MSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0HIGH_REGISTER register\n");
		return ERROR;
	}


	CH0 = (MSB_0 << 8);
	CH0 |= LSB_0;

	pthread_mutex_unlock(&lock_res);
	// printf("CH0 %d\n",CH0);
	return SUCCESS;


}

int read_channel_1()
{
	pthread_mutex_lock(&lock_res);
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA1LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}


	if (read(file_des_lux, &LSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1LOW_REGISTER register\n");
		return ERROR;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA1HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}


	if (read(file_des_lux, &MSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1HIGH_REGISTER register\n");
		return ERROR;
	}

	CH1 = (MSB_1 << 8);
	CH1 |= LSB_1;

	pthread_mutex_unlock(&lock_res);
	// printf("CH1 %d\n",CH1);
	return SUCCESS;

 
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
		printf("Invalid Lux measurement\n");
		return ERROR;
	}


}

void has_state_transition_occurred(float lux)
{
	static float prev_lux = 0;
	if((lux > 200) && (prev_lux <200))
	{
		printf("State changed from Dark to Bright\n");
		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"State changed from Dark to Bright\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	}
	else if((lux < 200) && (prev_lux > 200))
	{
		printf("State changed from Bright to Dark\n");
		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"State changed from Bright to Dark\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

		//clering the interrupt
		//command to write on control register  for Interupt register 
		register_data = 0xC0 ;

		if (write(file_des_lux, &register_data, 1) == ERROR)
		{
			perror("Error on writing the control register\n");
		}


	}
	prev_lux = lux;


}

float get_lux()
{
	usleep(500);
	if((read_channel_0() == ERROR) || (read_channel_1() == ERROR))
	{
		return ERROR;
	}

	else
	{
		
		return lux_measurement(CH0,CH1);
		
	}
}

enum Status get_current_state_fridge(float value)
{
	
	if(value > 200)
		return BRIGHT;
	if(value < 200)
		return DARK;
	else
		return ERROR;

}
