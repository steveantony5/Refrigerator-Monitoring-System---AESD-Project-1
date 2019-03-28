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

    sprintf(buffer,"[PID:%d] [TID:%lu]\n", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	setup_timer_POSIX(&timer_id_temp,temp_timer_handler);

	kick_timer(timer_id_temp, Delay_NS);

	mkfifo(Temp, 0666);

	int fd1 = open(Temp, O_WRONLY);

	temp_sensor_init();

	while(1)
	{
		if(FLAG_READ_TEMP)
		{
			// pthread_mutex_lock(&lock);
			printf("Test temp\n");
			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Temperatue in celcius = %f\n", temp_read() * 0.0625);
			printf("Temperatue in celcius = %f\n", temp_read() * 0.0625);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"T-high in celcius = %f\n", thigh_reg_read() * 0.0625);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"T-low in celcius = %f\n", tlow_reg_read() * 0.0625);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);


			write(fd1, "T", 1);

			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from temperature thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			FLAG_READ_TEMP = 0;
			// pthread_mutex_unlock(&lock);

		}
	}
	close(fd1);
}

/*****************************************************************
						lux_thread
*****************************************************************/
void *lux_task()
{
	float lux = 0;
	int reboot_tries = 0;

	char buffer[MAX_BUFFER_SIZE];

	sprintf(buffer,"[PID:%d] [TID:%lu]\n", getpid(), syscall(SYS_gettid));
	mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

	setup_timer_POSIX(&timer_id_lux,lux_timer_handler);
	kick_timer(timer_id_lux, Delay_NS);

	mkfifo(Lux, 0666);

	int fd2 = open(Lux, O_WRONLY); 

	reboot:
	reboot_tries++;
	if(reboot_tries ==10)
	{
		printf("Reboot failed multiple times\n");
		// exit(1);
	}

	if((i2c_setup(&file_des_lux,2,0x39)) != 0)
	{
		perror("Error on i2c bus set up for lux sensor");
		goto reboot;
	}

	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
		goto reboot;
	}


	while(1)
	{

		if(FLAG_READ_LUX)
		{
			// pthread_mutex_lock(&lock);
			printf("Test lux\n");
			if(read_channel_0()<0)
			{
				perror("Error on reading channel 0\n");
				goto reboot;
			}

			if(read_channel_1()<0)
			{
				perror("Error on reading channel 0\n");
				goto reboot;
			}

			printf("CH0 %d\n",CH0);
			printf("CH1 %d\n",CH1);

			lux = lux_measurement(CH0,CH1);
			printf("lux %f\n",lux);


			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Lux = %f\n",lux);
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			write(fd2, "L", 1);
			memset(buffer,0,MAX_BUFFER_SIZE);
			sprintf(buffer,"Pulse from lux thread\n");
			mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

			FLAG_READ_LUX = 0;
			// pthread_mutex_unlock(&lock);
		}
	}
	close(fd2);
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

	// if (pthread_mutex_init(&lock, NULL) != 0) 
 //    { 
 //        perror("Mutex init failed\n"); 
 //        return -1; 
 //    }

	file_ptr = fopen(file_name, "a+");


	pthread_attr_t attr;
	pthread_attr_init(&attr); 

	pthread_create(&logger_thread, &attr, logger_thread_callback, (void *)&fd);
	
	pthread_create(&temperature_thread, &attr, temperature_task, NULL);	

	pthread_create(&lux_thread, &attr, lux_task, NULL);	

	pthread_create(&remote_request_thread, &attr, remote_request_callback, (void *)&fd);


	mkfifo(Temp, 0666);
	mkfifo(Lux, 0666);
	mkfifo(log_t, 0666);

	int fd1 = open(Temp,O_RDONLY);
	int fd2 = open(Lux,O_RDONLY); 
	int fd3 = open(log_t,O_RDONLY); 

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

		/*
		if(Pulse_temp > 20)
			pthread_cancel(temperature_thread);
*/
		
	}

	//wait till the child completes
	pthread_join(temperature_thread,NULL);
	pthread_join(lux_thread,NULL);

	pthread_join(logger_thread, NULL);
	pthread_join(remote_request_thread, NULL);

	fclose(file_ptr);

	return 0;

}

