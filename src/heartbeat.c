/*****************************************************************
						Includes
*****************************************************************/
#include "heartbeat.h"

/*****************************************************************
						Globals
*****************************************************************/
timer_t timer_id_temp, timer_id_lux , timer_id_heartbeat;

pthread_t temperature_thread , lux_thread;

extern pthread_mutex_t lock_res;

pid_t pid;

/*Variables to store the heartbeat count from the threads*/
int Pulse_temp = 0;
int Pulse_lux = 0;
int Pulse_log = 0;

/*variables to store the previous heart beat received so as to find if the thread is alive*/
int Pulse_temp_prev = 0;
int Pulse_lux_prev = 0;
int Pulse_log_prev = 0;

//flags set in the timers
int FLAG_READ_TEMP = 0;
int FLAG_READ_LUX = 0;

/*flags for start up tests*/
int logger_thread_creation = 0;
int remote_socket_thread_creation = 0;
int temperature_thread_creation = 0;
int lux_thread_creation = 0;

int temp_dead_flag = 0;
int lux_dead_flag = 0;
int remote_socket_dead_flag = 0;
int logger_dead_flag = 0;

volatile int start_temp_thread = 0;
volatile int start_lux_thread = 0;
int start_logger_thread = 0;

/*******************************
	Globals Temp thread
********************************/
int fd1_w; /*for dumping the heart beat to a pipe*/

/*******************************
	Globals Lux thread
********************************/
int fd2_w; /*for dumping the heart beat to a pipe*/

/*******************************
	Globals heart beat - main 
********************************/
int fd1, fd2, fd3; /*for dumping the heart beat to a pipe*/

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
    sprintf(buffer,"INFO [PID:%d] [TID:%lu]", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	/*creating a timer for temperature task*/
	if((setup_timer_POSIX(&timer_id_temp,temp_timer_handler)) == ERROR)
	{
		perror("Error on creating timer for temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR creating timer for temp failed- Killed temp thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}

	if((kick_timer(timer_id_temp, Delay_NS)) == ERROR)
	{
		perror("Error on kicking timer for temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR kicking timer for temp failed- Killed temp thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}
	

	/*opening a pipe to send the heartbeat to the main function*/
	fd1_w = open(Temp, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
	if(fd1_w == ERROR)
	{
		perror("Error on creating FIFO fd1_w for Temp\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in temperature sensor init- Killed temp thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(temperature_thread); 
	}

	/*initiating the temperature sensor*/
	if(temp_sensor_init() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in temperature sensor init");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	/*Registers to set threshold value*/
	if(tlow_reg_write(27) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in temperature sensor threshold set");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(thigh_reg_write(29) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in temperature sensor threshold set");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	

	/*configuration registers*/
	if(config_reg_write_default() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"WARN Failed in temperature config_reg_write_default");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_tm_interrupt() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"WARN Failed in temperature config_tm_interrupt");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_fault_bits_6() == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"WARN Failed in temperature config_fault_bits_6");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(config_reg_read(&configuration) == ERROR)
	{
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"WARN Failed in temperature config_reg_read");
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
				sprintf(buffer,"ERROR on sending temp heartbeat");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			/*logging the heartbeat*/
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			sprintf(buffer,"DEBUG Pulse from temperature thread");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			/*reading the data register*/
			if(temp_read() == TEMP_ERROR || thigh_reg_read() == TEMP_ERROR || tlow_reg_read() == TEMP_ERROR)
			{
				led_on(1);
				
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				sprintf(buffer,"ERROR Temperatue sensor error,  trying to reconnect");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{

				led_off(1);
				
				float temperature_celcius = temp_read() * 0.0625;
				memset(buffer,'\0',MAX_BUFFER_SIZE);
				sprintf(buffer,"INFO Temperatue in celcius = %f", temperature_celcius);
				printf("Temperatue in celcius = %f\n", temperature_celcius);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


				memset(buffer,'\0',MAX_BUFFER_SIZE);
				sprintf(buffer,"DEBUG T-high in celcius = %f", thigh_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,'\0',MAX_BUFFER_SIZE);
				sprintf(buffer,"DEBUG T-low in celcius = %f", tlow_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				// /*setting temperature alert*/
				// int alert = config_read_alert();
				
				// if(alert == 1)
				// 	led_off(2);
				// else
				// 	led_on(2);

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
	sprintf(buffer,"INFO [PID:%d] [TID:%lu]", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	/*setting the timer for lux thread*/
	if((setup_timer_POSIX(&timer_id_lux,lux_timer_handler)) == ERROR)
	{
		perror("Error on creating timer for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR creating timer for lux failed- Killed lux thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}

	if((kick_timer(timer_id_lux, Delay_NS)) == ERROR)
	{
		perror("Error on kicking timer for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR kicking timer for lux failed- Killed lux thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}
	


	/*opening a pipe to dump the heartbeat from lux thread to maintask*/
	fd2_w = open(Lux, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);
	if(fd2_w == ERROR)
	{
		perror("Error on creating FIFO fd2_w for lux\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in lux sensor init- Killed lux thread");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		pthread_cancel(lux_thread); 
	}

	/*setting up the lux sensor*/
	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Failed in lux sensor setup");
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
				sprintf(buffer,"ERROR on sending lux heartbeat");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}
			

			/*logging the heartbeat*/
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			sprintf(buffer,"DEBUG Pulse from lux thread");
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
				sprintf(buffer,"ERROR Lux sensor error,  trying to reconnect");
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
				sprintf(buffer,"INFO Lux = %f",lux);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				/*get the current state of the fridge door
				 whether it is opened or closed*/
				fridge_state  = get_current_state_fridge(lux);
				if(fridge_state == BRIGHT)
				{
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					sprintf(buffer,"DEBUG Fridge state - Door opened");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == DARK)
				{
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					sprintf(buffer,"DEBUG Fridge state - Door Closed");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == ERROR)
				{
					printf("Fridge in unknown state\n");
					memset(buffer,'\0',MAX_BUFFER_SIZE);
					sprintf(buffer,"DEBUG Fridge state - unknown");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
				}

			}

			// #ifdef DEBUG
			// printf("CH0 %d\n",CH0);

			// #endif

			/*clearing the flag which will be set when the timer of lux will be triggered*/
        	FLAG_READ_LUX = 0;
		}
	}
	close(fd2_w);
	return SUCCESS;
}
/*****************************************************************
					Heart_beat checker
*****************************************************************/
void beat_timer_handler(union sigval val)
{
	char buffer[MAX_BUFFER_SIZE];

	#ifdef DEBUG
	printf("L p:%d c:%d\n",Pulse_lux_prev,Pulse_lux);
	printf("T p:%d c:%d\n",Pulse_temp_prev,Pulse_temp);
	printf("G p:%d c:%d\n",Pulse_log_prev,Pulse_log);
	#endif

	/*Check liveliness of temperature thread*/
	if(Pulse_temp <= Pulse_temp_prev)
	{
		printf("Temp thread dead\n");

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Temp thread dead");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	}

	/*Check liveliness of logger thread*/
	if(Pulse_log <= Pulse_log_prev)
	{
		printf("Log thread dead\n");

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Log thread dead");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	/*Check liveliness of lux thread*/
	if(Pulse_lux <= Pulse_lux_prev)
	{
		printf("Lux thread dead\n");

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Lux thread dead");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	/*Assigning current heart beat values as previous values*/
	Pulse_lux_prev = Pulse_lux;
	Pulse_temp_prev = Pulse_temp;
	Pulse_log_prev = Pulse_log;

	//restarting the heartbeat timer
	if((kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD)) == ERROR)
	{
		perror("Error on kicking timer for heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR kicking timer for heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	
}

/*****************************************************************
					Power On Self Test - POST
*****************************************************************/
int startup_test()
{
	int ret_val;

	/*Checks the temperature sensor power on check*/ 
	ret_val = temp_sensor_init();
	if(ret_val == ERROR)
	{
		perror("Satrup test temperature init failed");
		temp_dead_flag = 1;
	}

	ret_val = (int)temp_in_celcius();
	if(ret_val <-40 || ret_val > 128)
	{
		perror("Sartup temperature value test failed");
		temp_dead_flag = 1;
	}
	
	if(lux_sensor_setup() == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}

	/*Checks the light sensor power on check*/
	if(indication_register() == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}
	
	ret_val = (int)get_lux();
	if(ret_val == ERROR)
	{
		perror("Sartup lux value test failed");
		lux_dead_flag = 1;
	}
	
	/*Checks if the threads are spawned properly*/
	if(!remote_socket_thread_creation)
	{
		perror("Sartup remote request thread creation test failed");
		remote_socket_dead_flag = 1;
	}
	
	if(!temperature_thread_creation)
	{
		perror("Sartup temperature thread creation test failed");
		temp_dead_flag = 1;
	}
	

	if(!lux_thread_creation)
	{
		perror("Sartup lux thread creation test failed");
		lux_dead_flag = 1;
	}
	
	if(!logger_thread_creation)
	{
		perror("Sartup logger thread creation test failed");
		logger_dead_flag = 1;
	}
	
	if(!lux_dead_flag)
	{
		start_lux_thread = 1;
	}	
	

	if(!temp_dead_flag)
	{
		start_temp_thread = 1;
	}
	
	/*returns error code for any failure in POST*/
	if(temp_dead_flag || lux_dead_flag || remote_socket_dead_flag || logger_dead_flag)
	{
		return ERROR;
	}
	

	return SUCCESS;
}

/***********************************************
			Main Function
***********************************************/

#if NO_UNIT_TEST
int main(int argc, char *argv[])
{

	if(argc < 3)
	{
		perror("Please enter the <log file name> follwed by <log file folder name>");
		exit(ERROR);
	}

	int8_t i = 0;

	/*get the process id of the process*/
	pid = getpid();
	printf("\n\nPID of the process - %d\n",getpid());

	/*Assigned a signal handler for each thread to control it*/
	signal(SIGUSR1,hanler_kill_temp);
	signal(SIGUSR2,hanler_kill_lux);
	signal(SIGTERM,hanler_kill_main);
	signal(SIGALRM,hanler_kill_log);

	file_descriptors fd;
	
	/*gets the logger file name and log file folder name from command line argments*/
	fd.file_name = argv[1];
	fd.file_path = argv[2];


	mkdir(LOG_PATH,ACCESSPERMS);
	
	char file_path[MAX_BUFFER_SIZE];
	sprintf(file_path,"%s%s",LOG_PATH,"log");

	/*creating folder for logger file*/
	mkdir(file_path,ACCESSPERMS); // ACCESSPERMS = 0777
	
	
	char file_name[MAX_BUFFER_SIZE];
	sprintf(file_name,"%s%s/%s",LOG_PATH, fd.file_path, fd.file_name);

	/*creating the looger file*/
	logger_init(file_name);

	for(i=0; i<4; i++)
	{
		gpio_pin_init(i);
		led_off(i);
	}

	if (pthread_mutex_init(&lock_res, NULL) != 0) 
    { 
        perror("Mutex init failed\n"); 
        return ERROR; 
    }

	file_ptr = fopen(file_name, "a+");
	if(file_ptr == NULL)
	{
		perror("Error on creating log file\n");
	}


	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	/*Spawning the logger thread*/
	if(pthread_create(&logger_thread, &attr, logger_thread_callback, (void *)&fd) != 0)
	{
		perror("Logger thread creation failed");
	}
	else
	{
		logger_thread_creation = 1;
	}

	/*Spawning the temperature thread*/
	if(pthread_create(&temperature_thread, &attr, temperature_task, NULL) != 0) 	
	{
		perror("Temperature thread creation failed");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Temperature thread creation failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	else
	{
		temperature_thread_creation = 1;
	}
	
	/*Spawning the lux thread*/
	if(pthread_create(&lux_thread, &attr, lux_task, NULL) != 0)
	{
		perror("Lux thread creation failed");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Lux thread creation failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	else
	{
		lux_thread_creation = 1;
	}

	/*Spawning the remote request thread thread*/
	if(pthread_create(&remote_request_thread, &attr, remote_request_callback, (void *)&fd) != 0)
	{
		perror("Remote socket thread creation failed");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR Remote socket thread creation failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	else
	{
		remote_socket_thread_creation = 1;
	}


	/*Creating fifo to receive the heartbeat from the threads*/
	fd1 = open(Temp,O_RDONLY | O_NONBLOCK | O_CREAT, 0666   );
	if(fd1 < 0)
	{
		perror("error on opening fd1 Temp heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR opening fd1 FIFO Temp heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
       	

	fd2 = open(Lux,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd2 < 0)
	{
		perror("error on opening fd2 Lux heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR opening fd2 FIFO lux heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
       	

    fd3 = open(log_t,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd3 < 0)
	{
		perror("error on opening fd3 Lux heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR opening fd3 FIFO log heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}


    /*initiating start up tests*/
    int ret_val = startup_test();
	if(!ret_val)
	{
		#ifdef DEBUG
		printf("Startup test passed!\n");
		#endif

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"INFO Start up test Passed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	/*Killing the thread which failed on Power On Self Test*/
	else
	{
		if(temp_dead_flag)
		{
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			sprintf(buffer,"ERROR Temperature start up test failed");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			led_on(1);
			kill(pid, SIGUSR1);
		}
		if(lux_dead_flag)
		{
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			sprintf(buffer,"ERROR Lux start up test failed");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			led_on(0);
			kill(pid, SIGUSR2);
		}
		if(logger_dead_flag)
			kill(pid, SIGALRM);
	}

	char pulse[1];

	/*Creating a timer to check the liveliness of the threads at regular intervals*/
	if((setup_timer_POSIX(&timer_id_heartbeat,beat_timer_handler)) == ERROR)
	{
		perror("Error on creating timer for heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR creating timer for heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if((kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD)) == ERROR)
	{
		perror("Error on kicking timer for heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		sprintf(buffer,"ERROR kicking timer for heartbeat failed");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	
	

	/*Heat beat tracker*/
	while(1)
	{
		/*Checks if the heartbeat is received from temperature thread*/
		memset(pulse,0,1);
		if(read(fd1,pulse,2) > 0)
		{
			Pulse_temp++;
		}

		
		/*Checks if the heartbeat is received from lux thread*/
		memset(pulse,0,1);
		if(read(fd2,pulse,2) > 0)
		{
			Pulse_lux++;
		}

		/*Checks if the heartbeat is received from logger thread*/
		memset(pulse,0,1);
		if(read(fd3,pulse,2) > 0)
		{
			Pulse_log++;
		}

	}


	/*wait till the child completes*/
	pthread_join(temperature_thread,NULL);
	pthread_join(lux_thread,NULL);
	pthread_join(logger_thread, NULL);
	pthread_join(remote_request_thread, NULL);

	

	return SUCCESS;

}

#endif
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

/***********************************************
  Signal handler for killing main thread
***********************************************/
void hanler_kill_main(int num)
{

	printf("Encountered SIGTERM signal\nExiting main thread\n");
	
	kill(pid, SIGALRM);
	kill(pid, SIGUSR1);
	kill(pid, SIGUSR2);

	close(fd1);
	close(fd2);
	close(fd3);
	fclose(file_ptr);

	stop_timer(timer_id_heartbeat);

	/* remove the FIFOs */
    
    unlink(Lux);
    unlink(Temp);
    unlink(log_t);
    
    exit(SUCCESS);


	
}

/***********************************************
  Signal handler for killing logger thread
***********************************************/
void hanler_kill_log(int num)
{
	printf("Encountered SIGALRM signal\n");
	static int FLAG = 1;

	if((logger_thread_creation == 1) && (FLAG == 1))
	{
		printf("\nExiting log thread\n");
		mq_close(msg_queue);
    	mq_unlink(QUEUE_NAME);
    	close(fd3_w);
		stop_timer(timer_id_log);
		pthread_cancel(logger_thread); 
		FLAG = 0;
	}
	else
	{
		printf("Logger thread already dead\n");
	}

}
