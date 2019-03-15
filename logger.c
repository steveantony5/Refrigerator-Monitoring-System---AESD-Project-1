#include "logger.h"


const char *log_level[10] = {"INFO", "ERROR", "DEBUG"};

char *time_stamp()
{
	char *time_stamp;
	time_t timer;
	timer = time(NULL);
	strftime(time_stamp, 26, "%Y-%m-%d %H:%M:%S", localtime(&timer));
	return time_stamp;	
}


void logger_init(char *file_path)
{
	FILE *file_ptr;
	file_ptr = fopen(file_path, "w+");
	fprintf(file_ptr,"Queue Init\n\n");
    fclose(file_ptr);

    struct mq_attr mq_attributes;

    mq_attributes.mq_flags = 0;
    mq_attributes.mq_maxmsg = 10;
    mq_attributes.mq_msgsize = MAX_BUFFER_SIZE;
    mq_attributes.mq_curmsgs = 10;

    msg_queue = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &mq_attributes);
}

void *logger_thread_callback(void *arg)
{
	char buffer[MAX_BUFFER_SIZE];
    char file_name[MAX_BUFFER_SIZE];
    
    file_descriptors *fd = (file_descriptors *)arg;

    sprintf(file_name,"%s%s/%s",LOG_PATH,fd->file_path,fd->file_name);
   	printf("File name = %s\n",file_name);
    
   	
   	while(1)
   	{
	    if(mq_receive(msg_queue, buffer, MAX_BUFFER_SIZE, 0))
	    {
			LOG_MESSAGE(file_name,"%s %s %s\n", log_level[0], time_stamp(), buffer);
	    }
	}

    mq_close(msg_queue);
    mq_unlink(QUEUE_NAME);
}

