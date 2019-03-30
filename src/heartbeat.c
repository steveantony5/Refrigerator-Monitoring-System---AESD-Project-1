/*****************************************************************
						Includes
*****************************************************************/
#include "heartbeat.h"

/*****************************************************************
						Globals
*****************************************************************/
timer_t timer_id_temp, timer_id_lux , timer_id_heartbeat;

pthread_t temperature_thread , lux_thread;

pthread_mutex_t lock;

int Pulse_temp = 0;
int Pulse_lux = 0;
int Pulse_log = 0;

int Pulse_temp_prev = 0;
int Pulse_lux_prev = 0;
int Pulse_log_prev = 0;

int FLAG_READ_TEMP = 0;
int FLAG_READ_LUX = 0;


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


	int fd1_w = open(Temp, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);

	temp_sensor_init();

	tlow_reg_write(10);

	thigh_reg_write(100);

	config_reg_write_default();

	config_reg_read(&configuration);

	while(1)
	{
		if(FLAG_READ_TEMP)
		{
			
			write(fd1_w, "T", 1);

			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from temperature thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			pthread_mutex_lock(&lock);

			if(temp_read() == -1)
			{
				printf("Temperatue sensor error, trying to reconnect\n");
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Temperatue sensor error,  trying to reconnect");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{
				float temperature_celcius = temp_read() * 0.0625;
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Temperatue in celcius = %f\n", temperature_celcius);
				//printf("Temperatue in celcius = %f\n", temp_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"T-high in celcius = %f\n", thigh_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"T-low in celcius = %f\n", tlow_reg_read() * 0.0625);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			pthread_mutex_unlock(&lock);

			
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


	int fd2_w = open(Lux, O_WRONLY | O_NONBLOCK | O_CREAT, 0666);

	
	if((i2c_setup(&file_des_lux,2,0x39)) != 0)
	{
		perror("Error on i2c bus set up for lux sensor");
		// goto end;
	}

	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
		// goto end;
	}

	while(1)
	{

		if(FLAG_READ_LUX)
		{

			
			write(fd2_w, "L", 1);
			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from lux thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			pthread_mutex_lock(&lock);

			if(read_channel_0() < 0 || read_channel_1() < 0)
			{
				perror("Error on reading channels\n");
				printf("LUx sensor error, trying to reconnect\n");
				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"Lux sensor error,  trying to reconnect");
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			else
			{

				lux = lux_measurement(CH0,CH1);
				//printf("lux %f\n",lux);

				has_state_transition_occurred(lux);

				memset(buffer,0,MAX_BUFFER_SIZE);
				sprintf(buffer,"CH0 %d\nCH1 %d\nLux = %f\n",CH0,CH1,lux);
				mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
			}

			//printf("CH0 %d\n",CH0);
			//printf("CH1 %d\n",CH1);

			pthread_mutex_unlock(&lock);

		
        	FLAG_READ_LUX = 0;
		}
	}
	// end:
	close(fd2_w);
	printf("Cacnelling lux thread\n");
	// pthread_cancel(lux_thread);
	return 0;
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
		return 1;
	}

	ret_val = temp_in_celcius();
	if(ret_val <-40 && ret_val > 128)
	{
		perror("Satrup temperature value test failed");
		return 1;
	}

	return 0;
}

/***********************************************
			Main Function
***********************************************/

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		perror("Enter more arguments in terminal");
		exit(0);
	}
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

	if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        perror("Mutex init failed\n"); 
        return -1; 
    }

	file_ptr = fopen(file_name, "a+");


	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	pthread_create(&logger_thread, &attr, logger_thread_callback, (void *)&fd);
	
	pthread_create(&temperature_thread, &attr, temperature_task, NULL);	

	pthread_create(&lux_thread, &attr, lux_task, NULL);	



	int fd1 = open(Temp,O_RDONLY | O_NONBLOCK | O_CREAT, 0666   );
	if(fd1 < 0)
       	perror("error on opening fd1 Temp heartbeat\n");

	int fd2 = open(Lux,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd2 < 0)
       	perror("error on opening fd2 Lux heartbeat\n");

    int fd3 = open(log_t,O_RDONLY | O_NONBLOCK | O_CREAT, 0666  ); 
	if(fd3 < 0)
       	perror("error on opening fd3 Lux heartbeat\n");

	char pulse[1];

	setup_timer_POSIX(&timer_id_heartbeat,beat_timer_handler);
	kick_timer(timer_id_heartbeat, HEART_BEAT_CHECK_PERIOD);
	
	if(!startup_test())
		printf("Startup test passed!\n");

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

		/*
		if(Pulse_temp > 20)
			pthread_cancel(temperature_thread);
*/
		
	}

	//wait till the child completes
	pthread_join(temperature_thread,NULL);
	pthread_join(lux_thread,NULL);

	pthread_join(logger_thread, NULL);

	close(fd1);
	close(fd2);
	close(fd3);

	fclose(file_ptr);

	return 0;

}