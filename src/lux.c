/*****************************************************************
						Includes
*****************************************************************/
#include "lux.h"

/*****************************************************************
						Global definitions
*****************************************************************/
/*lux descriptor in i2c*/
int file_des_lux;

/*for writing and reading as byte from the registers*/
uint8_t register_data;

/*for writing and reading as word from the registers*/
uint16_t register_data_word;

/*for storing MSB and LSB of CH0 of lux*/
uint16_t MSB_0;
uint16_t LSB_0;

/*for storing MSB and LSB of CH1 of lux*/
uint16_t MSB_1;
uint16_t LSB_1;

/*16 bit value of CH0 and CH1*/
uint16_t CH0;
uint16_t CH1;


/*****************************************************************
					setting up lux sensor
*****************************************************************/
int lux_sensor_setup()
{

	pthread_mutex_lock(&lock_res);

	/*i2c setup*/
	if((i2c_setup(&file_des_lux,2,0x39)) == ERROR)
	{
		perror("Error on i2c bus set up for lux sensor");
		return ERROR;
	}

	/*command to write on control register*/
	register_data = WRITE_COMMAND | CONTROL_REGISTER |CLEAR_PENDING_INTERUPTS;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*Writing to control register*/
	register_data = 0x03;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*command to write on TIMING_REGISTER*/
	register_data = WRITE_COMMAND | TIMING_REGISTER;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*Writing to timing register*/
	register_data = 0x12;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the timing register\n");
		return ERROR;
	}

	
	/*command to write as a word for high threshold register */
	register_data = WRITE_COMMAND_WORD | THRESHHIGHLOW;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*Writing to threshold register*/
	register_data_word = 0x0BB8; //set to 3000

	if (write(file_des_lux, &register_data_word, 2) == ERROR)
	{
		perror("Error on writing the high threshold register\n");
		return ERROR;
	}

	/*command to write for INTERRUPT register */
	register_data = WRITE_COMMAND | INTERRUPT | CLEAR_PENDING_INTERUPTS;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*Writing to INTERRUPT register*/
	register_data = 0x12; 

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the interrupt register\n");
		return ERROR;
	}

	pthread_mutex_unlock(&lock_res);

	return SUCCESS;

}

/*****************************************************************
			Indication register for start up
*****************************************************************/
int indication_register()
{
	pthread_mutex_lock(&lock_res);
	
	/*command to write on INDICATION_REGISTER*/
	register_data = WRITE_COMMAND | INDICATION_REGISTER;

	if (write(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the control register\n");
		return ERROR;
	}

	/*Reading from INDICATION_REGISTER*/
	if (read(file_des_lux, &register_data, 1) == ERROR)
	{
		perror("Error on writing the interrupt register\n");
		return ERROR;
	}

	pthread_mutex_unlock(&lock_res);


	/*logging part numer and revision number of lux sensor*/
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

/*****************************************************************
			         Reading CH0
*****************************************************************/
int read_channel_0()
{

	pthread_mutex_lock(&lock_res);

	/*command to read on DATA0LOW register*/
	register_data = WRITE_COMMAND | DATA0LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}

	/*reading CH0 value lower byte*/
	if (read(file_des_lux, &LSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0LOW_REGISTER register\n");
		return ERROR;
	}

	/*command to read on DATA0HIGH register*/
	register_data = WRITE_COMMAND | DATA0HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}

	/*reading CH0 value upper byte*/
	if (read(file_des_lux, &MSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0HIGH_REGISTER register\n");
		return ERROR;
	}

	/*forming the full 16 bit from MSB and LSB*/
	CH0 = (MSB_0 << 8);
	CH0 |= LSB_0;

	pthread_mutex_unlock(&lock_res);

	return SUCCESS;


}

/*****************************************************************
			         Reading CH1
*****************************************************************/
int read_channel_1()
{
	pthread_mutex_lock(&lock_res);

	/*command to read on DATA1LOW register*/
	register_data = WRITE_COMMAND | DATA1LOW_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}

	/*reading CH1 value lower byte*/
	if (read(file_des_lux, &LSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1LOW_REGISTER register\n");
		return ERROR;
	}

	/*command to read on DATA1HIGH register*/
	register_data = WRITE_COMMAND | DATA1HIGH_REGISTER;

	if (write(file_des_lux, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return ERROR;
	}

	/*reading CH1 value upper byte*/
	if (read(file_des_lux, &MSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1HIGH_REGISTER register\n");
		return ERROR;
	}

	/*forming the full 16 bit from MSB and LSB*/
	CH1 = (MSB_1 << 8);
	CH1 |= LSB_1;

	pthread_mutex_unlock(&lock_res);
	// printf("CH1 %d\n",CH1);
	return SUCCESS;

 
}
  
/*****************************************************************
			         Getting lux value
*****************************************************************/
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

/*****************************************************************
			         Checks for state transition
*****************************************************************/
void has_state_transition_occurred(float lux)
{
	static float prev_lux = 0;
	if((lux > THRESHOLD_FOR_STATE_CHANGE) && (prev_lux <THRESHOLD_FOR_STATE_CHANGE))
	{
		printf("State changed from Dark to Bright\n");
		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"State changed from Dark to Bright\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	}
	else if((lux < THRESHOLD_FOR_STATE_CHANGE) && (prev_lux > THRESHOLD_FOR_STATE_CHANGE))
	{
		printf("State changed from Bright to Dark\n");
		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"State changed from Bright to Dark\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

		// //clearing the interrupt
		// //command to write on control register  for Interupt register 
		// register_data = CLEAR_PENDING_INTERUPTS | WRITE_COMMAND ;

		// if (write(file_des_lux, &register_data, 1) == ERROR)
		// {
		// 	perror("Error on writing the control register\n");
		// }


	}

	prev_lux = lux;


}


/*****************************************************************
			         Wrapper for lux measurement
*****************************************************************/
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


/*****************************************************************
			        Get the state for fridge door
*****************************************************************/
enum Status get_current_state_fridge(float value)
{
	
	if(value > THRESHOLD_FOR_STATE_CHANGE)
		return BRIGHT;
	if(value <= THRESHOLD_FOR_STATE_CHANGE)
		return DARK;
	else
		return ERROR;

}
