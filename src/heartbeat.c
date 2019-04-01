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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Temp thread dead", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	}

	/*Check liveliness of logger thread*/
	if(Pulse_log <= Pulse_log_prev)
	{
		printf("Log thread dead\n");

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Log thread dead", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	/*Check liveliness of lux thread*/
	if(Pulse_lux <= Pulse_lux_prev)
	{
		printf("Lux thread dead\n");

		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Lux thread dead", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s kicking timer for heartbeat failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Temperature thread creation failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Lux thread creation failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s Remote socket thread creation failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s opening fd1 FIFO Temp heartbeat failed", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
       	

	fd2 = open(Lux,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd2 < 0)
	{
		perror("error on opening fd2 Lux heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s opening fd2 FIFO lux heartbeat failed", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
       	

    fd3 = open(log_t,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd3 < 0)
	{
		perror("error on opening fd3 Lux heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s opening fd3 FIFO log heartbeat failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"INFO %s Start up test Passed", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}
	/*Killing the thread which failed on Power On Self Test*/
	else
	{
		if(temp_dead_flag)
		{
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			SOURCE_ID(source_id_buffer);
			sprintf(buffer,"ERROR %s Temperature start up test failed", source_id_buffer);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			led_on(1);
			kill(pid, SIGUSR1);
		}
		if(lux_dead_flag)
		{
			memset(buffer,'\0',MAX_BUFFER_SIZE);
			SOURCE_ID(source_id_buffer);
			sprintf(buffer,"ERROR %s Lux start up test failed", source_id_buffer);
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
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s creating timer for heartbeat failed", source_id_buffer);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
	}

	if((kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD)) == ERROR)
	{
		perror("Error on kicking timer for heartbeat\n");
		memset(buffer,'\0',MAX_BUFFER_SIZE);
		SOURCE_ID(source_id_buffer);
		sprintf(buffer,"ERROR %s kicking timer for heartbeat failed", source_id_buffer);
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
