#include "main.h"


pthread_t thread2;
pthread_t thread1;

void *thread2_callback(void *arg)
{
	int i;
	char buffer[MAX_BUFFER_SIZE];

	while(i<5)
	{
		
		sprintf(buffer,"[PID:%d] [TID:%lu] Temperature value: %d", getpid(), syscall(SYS_gettid), i);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		i++;
		sleep(1);
	}
}

void *thread1_callback(void *arg)
{
	int i;
	char buffer[MAX_BUFFER_SIZE];

	while(i<5)
	{
		
		sprintf(buffer,"[PID:%d] [TID:%lu] Lux value: %d", getpid(), syscall(SYS_gettid), i);
		mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
		i++;
		sleep(1);
	}
}

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

	printf("path = %s\n",fd.file_path);

	mkdir(LOG_PATH,ACCESSPERMS);
	
	char file_path[MAX_BUFFER_SIZE];
	sprintf(file_path,"%s%s",LOG_PATH,"log");
	mkdir(file_path,ACCESSPERMS); // ACCESSPERMS = 0777
	
	printf("File path = %s\n",file_path);
	
	char file_name[MAX_BUFFER_SIZE];
	sprintf(file_name,"%s%s/%s",LOG_PATH, fd.file_path, fd.file_name);
	printf("File name = %s\n", file_name);

	logger_init(file_name);

	file_ptr = fopen(file_name, "a+");
	
	pthread_attr_t attribute;
	pthread_attr_init(&attribute);

	pthread_create(&logger_thread, &attribute, logger_thread_callback, (void *)&fd);
	
	pthread_create(&thread1, &attribute, thread1_callback, (void *)&fd);

	pthread_create(&thread2, &attribute, thread2_callback, (void *)&fd);
	


	pthread_join(logger_thread, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	fclose(file_ptr);
	

	return 0;
}