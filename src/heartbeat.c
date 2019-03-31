/*****************************************************************
						Includes
*****************************************************************/
#include "heartbeat.h"

/*****************************************************************
						Globals
*****************************************************************/
timer_t timer_id_temp, timer_id_lux , timer_id_heartbeat;

pthread_t temperature_thread , lux_thread;

pthread_mutex_t lock_res;

int Pulse_temp = 0;
int Pulse_lux = 0;
int Pulse_log = 0;

int Pulse_temp_prev = 0;
int Pulse_lux_prev = 0;
int Pulse_log_prev = 0;

int FLAG_READ_TEMP = 0;
int FLAG_READ_LUX = 0;

int logger_thread_creation = 0;
int remote_socket_thread_creation = 0;
int temperature_thread_creation = 0;
int lux_thread_creation = 0;


int temp_dead_flag = 0;
int lux_dead_flag = 0;
int remote_socket_dead_flag = 0;
int logger_dead_flag = 0;
/*******************************
	Globals Temp thread
********************************/
int fd1_w;

/*******************************
	Globals Lux thread
********************************/
int fd2_w;

/*******************************
	Globals heart beat - main 
********************************/
int fd1, fd2, fd3;

/*****************************************************************
						temperature_thread
*****************************************************************/
void *temperature_task()
{
	char buffer[MAX_BUFFER_SIZE];

	uint16_t configuration;


    sprintf(buffer,"[PID:%d] [TID:%lu]\n", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	setup_timer_POSIX(&timer_id_temp,temp_timer_handler);

	kick_timer(timer_id_temp, Delay_NS);


	fd1_w = open(Temp, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);

	temp_sensor_init();

	tlow_reg_write(25);

	thigh_reg_write(27);

	config_reg_write_default();

	config_reg_read(&configuration);

	// thigh_reg_write(100);

	while(1)
	{
		if(FLAG_READ_TEMP)
		{
			
			write(fd1_w, "T", 1);

			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from temperature thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			// pthread_mutex_lock(&lock_res);

			if(temp_read() == ERROR)
			{
				// led_on();
				// printf("LED ON TEMP\n");
				// printf("Temperatue sensor error, trying to reconnect\n");
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Temperatue sensor error,  trying to reconnect");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{
				// led_off();
				// printf("LED OFF TEMP\n");
				float temperature_celcius = temp_read() * 0.0625;
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Temperatue in celcius = %f\n", temperature_celcius);

				// printf("Temperatue in celcius = %f\n", temperature_celcius);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"T-high in celcius = %f\n", thigh_reg_read() * 0.0625);
				//printf("T-high in celcius = %f\n", thigh_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"T-low in celcius = %f\n", tlow_reg_read() * 0.0625);
				//printf("T-low in celcius = %f\n", tlow_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				int alert = config_read_alert();
				
				if(alert == 1)
					led_off();
				else
					led_on();
				//printf("Alert Pin state = %d\n",alert);
			}

			// pthread_mutex_unlock(&lock_res);

			
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
	float lux = 0;

	char buffer[MAX_BUFFER_SIZE];


	sprintf(buffer,"[PID:%d] [TID:%lu]\n", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	setup_timer_POSIX(&timer_id_lux,lux_timer_handler);
	kick_timer(timer_id_lux, Delay_NS);


	fd2_w = open(Lux, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);

	
	

	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
	}

	while(1)
	{

		if(FLAG_READ_LUX)
		{

			
			write(fd2_w, "L", 1);
			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from lux thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			// pthread_mutex_lock(&lock_res);

			usleep(500);
			if((read_channel_0() == ERROR) || (read_channel_1() == ERROR))
			{
				// led_on();
				// printf("LED ON LUX\n");
				perror("Error on reading channels\n");
				printf("Lux sensor error, trying to reconnect\n");
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Lux sensor error,  trying to reconnect");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{
				// led_off();
				// printf("LED OFF LUX\n");
				lux = lux_measurement(CH0,CH1);
				// printf("lux %f\n",lux);

				has_state_transition_occurred(lux);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"CH0 %d\nCH1 %d\nLux = %f\n",CH0,CH1,lux);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				fridge_state  = get_current_state_fridge(lux);
				if(fridge_state == BRIGHT)
				{
					//printf("Fridge Door is open\n");
					memset(buffer,0,MAX_BUFFER_SIZE);
					sprintf(buffer,"Fridge state - Door opened\n");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == DARK)
				{
					//printf("Fridge Door is close\n");
					memset(buffer,0,MAX_BUFFER_SIZE);
					sprintf(buffer,"Fridge state - Door Closed\n");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				}
				else if(fridge_state == ERROR)
				{
					printf("Fridge in unknown state\n");
					memset(buffer,0,MAX_BUFFER_SIZE);
					sprintf(buffer,"Fridge state - unknown\n");
					mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
				}

			}

			// printf("CH0 %d\n",CH0);
			//printf("CH1 %d\n",CH1);

			// pthread_mutex_unlock(&lock_res);

		
        	FLAG_READ_LUX = 0;
		}
	}
	// end:
	close(fd2_w);
	return SUCCESS;
}
/*****************************************************************
					Heart_beat checker
*****************************************************************/
void beat_timer_handler(union sigval val)
{
	char buffer[MAX_BUFFER_SIZE];

	printf("L p:%d c:%d\n",Pulse_lux_prev,Pulse_lux);
	printf("T p:%d c:%d\n",Pulse_temp_prev,Pulse_temp);
	printf("G p:%d c:%d\n",Pulse_log_prev,Pulse_log);

	if(Pulse_temp <= Pulse_temp_prev)
	{
		printf("Temp thread dead\n");

		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"Temp thread dead\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	}

	if(Pulse_log <= Pulse_log_prev)
	{
		printf("Log thread dead\n");

		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"Log thread dead\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if(Pulse_lux <= Pulse_lux_prev)
	{
		printf("Lux thread dead\n");

		memset(buffer,0,MAX_BUFFER_SIZE);
		sprintf(buffer,"Lux thread dead\n");
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	Pulse_lux_prev = Pulse_lux;
	Pulse_temp_prev = Pulse_temp;
	Pulse_log_prev = Pulse_log;

	kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD);
	
}

int startup_test()
{
	int ret_val;

	
	ret_val = temp_sensor_init();
	if(ret_val != 0)
	{
		perror("Satrup test temperature init failed");
		temp_dead_flag = 1;
	}

	ret_val = (int)temp_in_celcius();
	if(ret_val <-40 && ret_val > 128)
	{
		perror("Sartup temperature value test failed");
		temp_dead_flag = 1;
	}

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

	if(temp_dead_flag || lux_dead_flag || remote_socket_dead_flag || logger_dead_flag)
		return ERROR;
	
	return SUCCESS;
}

/***********************************************
			Main Function
***********************************************/

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		perror("Enter more arguments in terminal");
		exit(ERROR);
	}

	pid_t pid = getpid();
	printf("\n\nPID of the process - %d\n",getpid());

	signal(SIGUSR1,hanler_kill_temp);
	signal(SIGUSR2,hanler_kill_lux);
	signal(SIGTERM,hanler_kill_main);
	signal(SIGALRM,hanler_kill_log);

	file_descriptors fd;
	
	fd.file_name = argv[1];
	fd.file_path = argv[2];


	mkdir(LOG_PATH,ACCESSPERMS);
	
	char file_path[MAX_BUFFER_SIZE];
	sprintf(file_path,"%s%s",LOG_PATH,"log");
	mkdir(file_path,ACCESSPERMS); // ACCESSPERMS = 0777
	
	
	char file_name[MAX_BUFFER_SIZE];
	sprintf(file_name,"%s%s/%s",LOG_PATH, fd.file_path, fd.file_name);

	logger_init(file_name);
	gpio_pin_init();
	led_off();

	if (pthread_mutex_init(&lock_res, NULL) != 0) 
    { 
        perror("Mutex init failed\n"); 
        return ERROR; 
    }

	file_ptr = fopen(file_name, "a+");


	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	if(pthread_create(&logger_thread, &attr, logger_thread_callback, (void *)&fd) != 0)
	{
		perror("Logger thread creation failed");
	}
	else
	{
		logger_thread_creation = 1;
	}

	
	if(pthread_create(&temperature_thread, &attr, temperature_task, NULL) != 0) 	
	{
		perror("Temperature thread creation failed");
	}
	else
	{
		temperature_thread_creation = 1;
	}
	

	if(pthread_create(&lux_thread, &attr, lux_task, NULL) != 0)
	{
		perror("Lux thread creation failed");
	}
	else
	{
		lux_thread_creation = 1;
	}

	if(pthread_create(&remote_request_thread, &attr, remote_request_callback, (void *)&fd) != 0)
	{
		perror("Reomte socket thread creation failed");
	}
	else
	{
		remote_socket_thread_creation = 1;
	}

	fd1 = open(Temp,O_RDONLY | O_NONBLOCK | O_CREAT, 0666   );
	if(fd1 < 0)
       	perror("error on opening fd1 Temp heartbeat\n");

	fd2 = open(Lux,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd2 < 0)
       	perror("error on opening fd2 Lux heartbeat\n");

    fd3 = open(log_t,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd3 < 0)
       	perror("error on opening fd3 Lux heartbeat\n");

    sleep(1);
    int ret_val = startup_test();
	if(!ret_val)
		printf("Startup test passed!\n");
	else
	{
		if(temp_dead_flag)
			kill(pid, SIGUSR1);
		if(lux_dead_flag)
			kill(pid, SIGUSR2);
		if(logger_dead_flag)
			kill(pid, SIGALRM);
	}

	char pulse[1];

	setup_timer_POSIX(&timer_id_heartbeat,beat_timer_handler);
	kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD);
	
	while(1)
	{
		memset(pulse,0,1);
		if(read(fd1,pulse,2) > 0)
		{
			Pulse_temp++;
		}
		

		memset(pulse,0,1);
		if(read(fd2,pulse,2) > 0)
		{
			Pulse_lux++;
		}

		memset(pulse,0,1);
		if(read(fd3,pulse,2) > 0)
		{
			Pulse_log++;
		}

	}

	//wait till the child completes
	pthread_join(temperature_thread,NULL);
	pthread_join(lux_thread,NULL);
	pthread_join(logger_thread, NULL);
	pthread_join(remote_request_thread, NULL);

	

	return SUCCESS;

}

void hanler_kill_temp(int num)
{
	printf("Encountered SIGUSR1 signal\nExiting temperature thread\n");
	close(fd1_w);
	stop_timer(timer_id_temp);
	pthread_cancel(temperature_thread); 
}

void hanler_kill_lux(int num)
{
	printf("Encountered SIGUSR2 signal\nExiting lux thread\n");
	close(fd2_w);
	stop_timer(timer_id_lux);
	pthread_cancel(lux_thread); 

}

void hanler_kill_main(int num)
{
	printf("Encountered SIGTERM signal\nExiting main thread\n");
	close(fd1);
	close(fd2);
	close(fd3);
	fclose(file_ptr);

	stop_timer(timer_id_heartbeat);

	/* remove the FIFO */
    
    unlink(Lux);
    unlink(Temp);
    unlink(log_t);
    exit(SUCCESS);


	
}

void hanler_kill_log(int num)
{
	printf("Encountered SIGALRM signal\nExiting log thread\n");
	close(fd3_w);
	stop_timer(timer_id_log);
	pthread_cancel(logger_thread); 

}
