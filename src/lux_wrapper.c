/**
 * @\file	temp.c
 * @\author	Steve Antony X
 * @\brief	This files contains the definitions of wrapper functions for lux registers
 * @\date	03/29/2019
 *
 */
/*****************************************************************
						Includes
*****************************************************************/
#include "lux_wrapper.h"


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

/***********************************************
  Signal handler for killing lux thread
***********************************************/
void hanler_kill_lux(int num)
{
	printf("Encountered SIGUSR2 signal\n");
	static int FLAG = 1;

	if((lux_thread_creation == 1) && (FLAG == 1))
	{
		printf("Exiting lux thread\n");
		
		if(start_lux_thread)
		{
			close(fd2_w);
			stop_timer(timer_id_lux);
		}
		pthread_cancel(lux_thread);
		FLAG = 0;
	}
	else
	{
		printf("Lux thread already dead\n");
	}

}
/*****************************************************************
						lux_thread
*****************************************************************/
void *lux_task()
{

	while(!start_lux_thread);

	/*for stroing the lux value*/
	float lux = 0;

	char buffer[MAX_BUFFER_SIZE];

	/*log the process id and thread group id of the thread*/
	memset(buffer,'\0',MAX_BUFFER_SIZE);
	SOURCE_ID(source_id_buffer);
	sprintf(buffer,"INFO [PID:%d] [TID:%lu]", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	/*setting the timer for lux thread*/
	if((setup_timer_POSIX(&timer_id_lux,lux_timer_handler)) == ERROR)
	{
		perror("Error on creating timer for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s creating timer for lux failed- Killed lux thread", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}

	if((kick_timer(timer_id_lux, Delay_NS)) == ERROR)
	{
		perror("Error on kicking timer for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s kicking timer for lux failed- Killed lux thread", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}
	


	/*opening a pipe to dump the heartbeat from lux thread to maintask*/
	fd2_w = open(Lux, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
	if(fd2_w == ERROR)
	{
		perror("Error on creating FIFO fd2_w for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Failed in lux sensor init- Killed lux thread", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}

	/*setting up the lux sensor*/
	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %sFailed in lux sensor setup", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	while(1)
	{
		/*enters if the flag is set in timer of lux thread
		lux value will be read when the timer expires*/
		if(FLAG_READ_LUX)
		{

			/*sends heartbeat*/
			if((write(fd2_w, "L", 1)) == ERROR)
			{
				perror("Error on write of lux heartbeat\n");
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"ERROR %s on sending lux heartbeat", source_id_buffer);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}
			

			/*logging the heartbeat*/
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			SOURCE_ID(source_id_buffer);
			sprintf(buffer,"DEBUG %s Pulse from lux thread", source_id_buffer);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			/*introduced delay for adc conversion*/
			usleep(500);
			if((read_channel_0() == ERROR) || (read_channel_1() == ERROR))
			{
				led_on(0);
				// printf("LED ON LUX\n");
				perror("Error on reading channels\n");
				printf("Lux sensor error, trying to reconnect\n");
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"ERROR %s Lux sensor error,  trying to reconnect", source_id_buffer);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{
				led_off(0);
				// printf("LED OFF LUX\n");

				/*measuring lux value*/
				lux = lux_measurement(CH0,CH1);
				printf("lux %f\n",lux);

				/*checks if a transition occured from dark to bright
				 or bright to dark
				 the threshold value is set as 70*/
				has_state_transition_occurred(lux);

				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"INFO %s Lux = %f", source_id_buffer, lux);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				/*get the current state of the fridge door
				 whether it is opened or closed*/
				fridge_state  = get_current_state_fridge(lux);
				if(fridge_state == BRIGHT)
				{
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					SOURCE_ID(source_id_buffer);
					sprintf(buffer,"DEBUG %s Fridge state - Door opened", source_id_buffer);
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == DARK)
				{
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					SOURCE_ID(source_id_buffer);
					sprintf(buffer,"DEBUG %s Fridge state - Door Closed", source_id_buffer);
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == ERROR)
				{
					printf("Fridge in unknown state\n");
					SOURCE_ID(source_id_buffer);
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					sprintf(buffer,"DEBUG %s Fridge state - unknown", source_id_buffer);
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
				}

			}

			/*clearing the flag which will be set when the timer of lux will be triggered*/
        	FLAG_READ_LUX = 0;
		}
	}
	close(fd2_w);
	return SUCCESS;
}
/*****************************************************************
					setting up lux sensor
*****************************************************************/
int lux_sensor_setup()
{
	int ret_status;

	pthread_mutex_lock(&lock_res);
	ret_status = i2c_setup(&file_des_lux,2,0x39);
	pthread_mutex_unlock(&lock_res);

	/*i2c setup*/
	if(ret_status == ERROR)
	{
		perror("Error on i2c bus set up for lux sensor");
		return ERROR;
	}

	/**********************************/
	/*command to write on control register*/
	ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,COMMAND , &register_data, CLEAR_PENDING_INTERUPTS );
	if(ret_status == ERROR)
	{
		perror("Error on CONTROL_REGISTER of lux sensor");
		return ERROR;
	}

	/*Writing to control register*/
	register_data = 0x03;
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on CONTROL_REGISTER of  lux sensor");
		return ERROR;
	}

	/**********************************/
	/*command to write on TIMING_REGISTER*/
	ret_status = byte_access_lux_register(file_des_lux, TIMING_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on TIMING_REGISTER of  lux sensor");
		return ERROR;
	}
	
	/*Writing to timing register*/
	register_data = 0x12;
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on TIMING_REGISTER of  lux sensor");
		return ERROR;
	}
	
	/**********************************/
	/*command to write as a word for high threshold register */
	/*upper threshold is set to 3000*/
	ret_status = byte_access_lux_register(file_des_lux, THRESHHIGHLOW,COMMAND , &register_data, WRITE_COMMAND_WORD );
	if(ret_status == ERROR)
	{
		perror("Error on THRESHHIGHLOW of lux sensor");
		return ERROR;
	}

	/*Writing to threshold register*/
	register_data = 0xB8; 
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on THRESHHIGHLOW of lux sensor");
		return ERROR;
	}

	/**********************************/
	ret_status = byte_access_lux_register(file_des_lux, THRESHHIGHHIGH,COMMAND , &register_data, WRITE_COMMAND_WORD );
	if(ret_status == ERROR)
	{
		perror("Error on THRESHHIGHHIGH of lux sensor");
		return ERROR;
	}

	/*Writing to threshold register*/
	register_data = 0xBB; 
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on THRESHHIGHHIGH of lux sensor");
		return ERROR;
	}

	/**********************************/
	/*command to write for INTERRUPT register */
	ret_status = byte_access_lux_register(file_des_lux, INTERRUPT,COMMAND , &register_data, CLEAR_PENDING_INTERUPTS );
	if(ret_status == ERROR)
	{
		perror("Error on INTERRUPT_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*Writing to INTERRUPT register*/
	register_data = 0x12; 
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on INTERRUPT_REGISTER of lux sensor");
		return ERROR;
	}
	return SUCCESS;
	
}

/*****************************************************************
Indication register for start up to get part number and revision number of sensor
*****************************************************************/
int indication_register()
{
	
	int ret_status;

	ret_status = byte_access_lux_register(file_des_lux, INDICATION_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on INTERRUPT_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*Writing to INTERRUPT register*/
	register_data = 0x12; 
	ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on INTERRUPT_REGISTER of lux sensor");
		return ERROR;
	}


	/*logging part numer and revision number of lux sensor*/
	uint8_t part_no = register_data & (0xF0);
	part_no = part_no >> 4;
	printf("Part numer of Lux sensor %d\n",part_no);

	uint8_t rev_no = register_data & (0x0F);
	printf("Revision numer of Lux sensor %d\n",rev_no);


	memset(buffer,0,MAX_BUFFER_SIZE);
	SOURCE_ID(source_id_buffer);
	sprintf(buffer,"INFO %s LUX sensor PNO: %d Rev no %d",source_id_buffer,part_no,rev_no);
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


	return SUCCESS;
}

/*****************************************************************
			         Reading CH0
*****************************************************************/
int read_channel_0()
{

	int ret_status;

	ret_status = byte_access_lux_register(file_des_lux, DATA0LOW_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA0LOW_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*reading CH0 value lower byte*/
	ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA0LOW_REGISTER of lux sensor");
		return ERROR;
	}
	LSB_0 = 0;
	LSB_0 = register_data;

	ret_status = byte_access_lux_register(file_des_lux, DATA0HIGH_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA0HIGH_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*reading CH0 value lower byte*/
	ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA0HIGH_REGISTER of lux sensor");
		return ERROR;
	}

	MSB_0 = 0;
	MSB_0 = register_data;

	/*forming the full 16 bit from MSB and LSB*/
	CH0 = (MSB_0 << 8);
	CH0 |= LSB_0;


	return SUCCESS;


}

/*****************************************************************
			         Reading CH1
*****************************************************************/
int read_channel_1()
{
	int ret_status;

	ret_status = byte_access_lux_register(file_des_lux, DATA1LOW_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA1LOW_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*reading CH0 value lower byte*/
	ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA1LOW_REGISTER of lux sensor");
		return ERROR;
	}
	LSB_1 = 0;
	LSB_1 = register_data;

	ret_status = byte_access_lux_register(file_des_lux, DATA1HIGH_REGISTER,COMMAND , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA1HIGH_REGISTER of lux sensor");
		return ERROR;
	}
	
	/*reading CH0 value lower byte*/
	ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	if(ret_status == ERROR)
	{
		perror("Error on DATA1HIGH_REGISTER of lux sensor");
		return ERROR;
	}
	MSB_1 = 0;
	MSB_1 = register_data;

	/*forming the full 16 bit from MSB and LSB*/
	CH1 = (MSB_1 << 8);
	CH1 |= LSB_1;


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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"INFO %s State changed from Dark to Bright\n", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		led_on(3);

	}
	else if((lux < THRESHOLD_FOR_STATE_CHANGE) && (prev_lux > THRESHOLD_FOR_STATE_CHANGE))
	{
		printf("State changed from Bright to Dark\n");
		memset(buffer,0,MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"INFO State %s changed from Bright to Dark\n", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		led_off(3);
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

