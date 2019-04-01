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

/***********************************************
  Signal handler for killing temperature thread
***********************************************/
void hanler_kill_temp(int num)
{
	printf("Encountered SIGUSR1 signal\n");
	static int FLAG = 1;

	if((temperature_thread_creation == 1) && (FLAG == 1))
	{
		printf("Exiting temperature thread\n");
		if(start_temp_thread)
		{
			close(fd1_w);
			stop_timer(timer_id_temp);
		}
		pthread_cancel(temperature_thread);
		FLAG = 0;
	}
	else
	{
		printf("Temperature thread already dead\n");
	} 
}

/*****************************************************************
						temperature_thread
*****************************************************************/
void *temperature_task()
{
	while(!start_temp_thread);

	char buffer[MAX_BUFFER_SIZE];
	uint16_t configuration;


	/*To get the Process id and Thread Group ID and logging it*/
	memset(buffer,0,MAX_BUFFER_SIZE);
	SOURCE_ID(source_id_buffer);
    sprintf(buffer,"INFO %s", source_id_buffer);
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	/*creating a timer for temperature task*/
	if((setup_timer_POSIX(&timer_id_temp,temp_timer_handler)) == ERROR)
	{
		perror("Error on creating timer for temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s creating timer for temp failed- Killed temp thread",source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}

	if((kick_timer(timer_id_temp, Delay_NS)) == ERROR)
	{
		perror("Error on kicking timer for temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s kicking timer for temp failed- Killed temp thread", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}
	

	/*opening a pipe to send the heartbeat to the main function*/
	fd1_w = open(Temp, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
	if(fd1_w == ERROR)
	{
		perror("Error on creating FIFO fd1_w for Temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Failed in temperature sensor init- Killed temp thread", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}

	/*initiating the temperature sensor*/
	if(temp_sensor_init() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Failed in temperature sensor init", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	/*Registers to set threshold value*/
	if(tlow_reg_write(27) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Failed in temperature sensor threshold set", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(thigh_reg_write(29) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Failed in temperature sensor threshold set", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	

	/*configuration registers*/
	if(config_reg_write_default() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"WARN %s Failed in temperature config_reg_write_default", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_tm_interrupt() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"WARN %s Failed in temperature config_tm_interrupt", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_fault_bits_6() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"WARN %s Failed in temperature config_fault_bits_6", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_reg_read(&configuration) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"WARN %s Failed in temperature config_reg_read", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	
	#ifdef DEBUG
	printf("FAULT BITS AFTER CONFIG = %x\n",config_read_fault_bits());
	#endif

	int fd = open("/sys/class/gpio/gpio49/value", O_RDONLY);
	int timeout = 100;
	struct pollfd fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLPRI | POLLERR | POLLHUP | POLLNVAL;

	while(1)
	{
		/*enters if the flag is set in timer of temperature thread
		temperature will be read when the timer expires*/
		if(FLAG_READ_TEMP)
		{
			/*sending heartbeat to the main task*/
			if((write(fd1_w, "T", 1)) == ERROR)
			{
				perror("Error on write of temp heartbeat\n");
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"ERROR %s on sending temp heartbeat", source_id_buffer);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			/*logging the heartbeat*/
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			SOURCE_ID(source_id_buffer);
			sprintf(buffer,"DEBUG %s Pulse from temperature thread", source_id_buffer);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			/*reading the data register*/
			if(temp_read() == TEMP_ERROR || thigh_reg_read() == TEMP_ERROR || tlow_reg_read() == TEMP_ERROR)
			{
				led_on(1);
				
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"ERROR %s Temperatue sensor error,  trying to reconnect", source_id_buffer);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{

				led_off(1);
				
				float temperature_celcius = temp_read() * 0.0625;
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"INFO %s Temperatue in celcius = %f", source_id_buffer, temperature_celcius);
				printf("Temperatue in celcius = %f\n", temperature_celcius);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"DEBUG %s T-high in celcius = %f", source_id_buffer, thigh_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,'\0',MAX_BUFFER_SIZE);
				SOURCE_ID(source_id_buffer);
				sprintf(buffer,"DEBUG %s T-low in celcius = %f", source_id_buffer, tlow_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				
				int ret = poll(fds,1,timeout);
				char buffer_poll[2];

				
				if(ret > 0)
				{
					memset(buffer_poll,'\0',2);
				
					lseek(fds[0].fd,0, SEEK_SET);
					read(fds[0].fd, buffer_poll, 2);

					if(fds[0].revents & POLLPRI)
					{
						printf("\n\n\n VALUE CHANGED \n\n\n");
						led_on(2);
					}
				}
				else
				{
					led_off(2);
				}
			}

			/*clearing the flag which will be set when the timer of temperature will be triggered*/			
        	FLAG_READ_TEMP = 0;



		}
	}
	close(fd1_w);
}

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


	// printf("writeConfig = %x\n", writeConfig);

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	pthread_mutex_lock(&lock_res);
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	pthread_mutex_unlock(&lock_res);

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

	pthread_mutex_lock(&lock_res);
	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));
	pthread_mutex_unlock(&lock_res);

	if(ret_val == ERROR)
	{
		perror("Error on reading CONFIGURATION REGISTER");
		return ERROR;
	}

	/*Reads the required bits from the byte*/
	readBytes[byte] &= bit_mask;
	readBytes[byte] = readBytes[byte] >> shift;

	// printf("bits = %x\n", readBytes[byte]);

	return readBytes[byte];
}

int temp_sensor_init()
{
	pthread_mutex_lock(&lock_res);
	int ret_val = i2c_setup(&file_des_temp,2,TEMP_ADDR); 
	pthread_mutex_unlock(&lock_res);
	
	return ret_val;
}

int pointer_reg_write(pointer_reg reg)
{
	int8_t buffer = reg;

	pthread_mutex_lock(&lock_res);
	int ret_val = write(file_des_temp, &buffer, sizeof(buffer));
	pthread_mutex_unlock(&lock_res);

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

	pthread_mutex_lock(&lock_res);
	int ret_val = read(file_des_temp, &readBytes, sizeof(readBytes));
	pthread_mutex_unlock(&lock_res);

	if(ret_val == ERROR)
	{
		perror("Error on reading CONFIGURATION REGISTER");
		return ERROR;
	}

	config = (readBytes[0] << 8 | readBytes[1]);

	// printf("CONFIG REG READ = %x\n", config);

	*configuration = config;

	return SUCCESS;

}


int config_reg_write_default()
{
	pointer_reg_write(configReg);

	uint8_t higherByte = default_configuration >> 8;
	uint8_t lowerByte = (uint8_t)default_configuration;

	uint8_t writeBytes[3] = {configReg, higherByte, lowerByte};

	pthread_mutex_lock(&lock_res);
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	pthread_mutex_unlock(&lock_res);

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


int config_pol_alert_active_low()
{
	return config_reg_write_update(POL_MODE, 1);	
}


int config_pol_alert_active_high()
{
	return config_reg_write_update(POL_MODE, 0);	
}


int config_tm_comparator()
{
	return config_reg_write_update(TM_MODE, 1);	
}


int config_tm_interrupt()
{
	return config_reg_write_update(TM_MODE, 0);	
}


int config_fault_bits_1()
{
	int ret_val;
	ret_val = config_reg_write_update(F0, 1);
	
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(F1, 1);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
}


int config_fault_bits_2()
{
	int ret_val;

	ret_val = config_reg_write_update(F0, 0);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(F1, 1);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
}

int config_fault_bits_4()
{
	int ret_val;
	
	ret_val = config_reg_write_update(F0, 1);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(F1, 0);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
}


int config_fault_bits_6()
{
	int ret_val;
	
	ret_val = config_reg_write_update(F0, 0);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(F1, 0);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
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
	int ret_val;

	ret_val = config_reg_write_update(CR0, 1);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(CR1, 1);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;

}

int config_conversion_rate_1HZ()
{
	int ret_val;

	ret_val = config_reg_write_update(CR0, 0);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(CR1, 1);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
}

int config_conversion_rate_4HZ()
{
	int ret_val;

	ret_val = config_reg_write_update(CR0, 1);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(CR1, 0);
	if(ret_val == ERROR)
		return ERROR;

	return SUCCESS;
}

int config_conversion_rate_8HZ()
{
	int ret_val;

	ret_val = config_reg_write_update(CR0, 0);
	if(ret_val == ERROR)
		return ERROR;

	ret_val = config_reg_write_update(CR1, 0);
	if(ret_val == ERROR)
		return ERROR;

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

	pthread_mutex_lock(&lock_res);
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	pthread_mutex_unlock(&lock_res);

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
	
	pthread_mutex_lock(&lock_res);
	int ret_val = write(file_des_temp, &writeBytes, sizeof(writeBytes));
	pthread_mutex_unlock(&lock_res);

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

	pthread_mutex_lock(&lock_res);
	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	pthread_mutex_unlock(&lock_res);

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

	pthread_mutex_lock(&lock_res);
	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	pthread_mutex_unlock(&lock_res);

	if(ret_val == ERROR)
	{
		perror("Error on reading TLOW REGISTER");
		return TEMP_ERROR;
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

	pthread_mutex_lock(&lock_res);
	int ret_val = read(file_des_temp, &readBytes,sizeof(readBytes));
	pthread_mutex_unlock(&lock_res);

	if(ret_val == ERROR)
	{
		perror("Error on reading TEMPERATURE REGISTER");
		return TEMP_ERROR;
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
	int temperature = temp_read();
	if(temperature == TEMP_ERROR)
		return TEMP_ERROR;
	
	return (temperature * 0.0625);
}

float temp_in_kelvin()
{
	int temperature = temp_read();
	if(temperature == TEMP_ERROR)
		return TEMP_ERROR;
	
	return ((temperature * 0.0625) + 273.15);
}

float temp_in_fahrenheit()
{
	int temperature = temp_read();
	if(temperature == TEMP_ERROR)
		return TEMP_ERROR;
	
	return (((temperature * 0.0625) * (9/5)) + 32);
}