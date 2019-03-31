/**
 * @\file	POSIX_timer.c
 * @\author	Steve Antony X
 * @\brief	This files contains the function definitions for POSIX timer 
 * @\date	03/30/2019
 *
 */

/*****************************************************************
						Includes
*****************************************************************/
#include "POSIX_timer.h"

/*****************************************************************
						Global declaration
*****************************************************************/
// FIFO file path 
 char * Temp = "/tmp/tmp_to_main";
 char * Lux = "/tmp/lux_to_main";
 char * log_t = "/tmp/log_to_main";
 
/*****************************************************************
					POSIX Timer Handler
*****************************************************************/

void temp_timer_handler(union sigval val)
{
	FLAG_READ_TEMP = 1;

	kick_timer(timer_id_temp, Delay_NS);
}

/*****************************************************************
					POSIX Timer Handler
*****************************************************************/

void lux_timer_handler(union sigval val)
{

	FLAG_READ_LUX = 1;

	kick_timer(timer_id_lux, Delay_NS);
}

/*****************************************************************
					POSIX Timer Handler
*****************************************************************/

void log_timer_handler(union sigval val)
{
	FLAG_LOG = 1;

	kick_timer(timer_id_log, Delay_NS);
}

/*****************************************************************
					POSIX Timer configuration
*****************************************************************/
int setup_timer_POSIX(timer_t *timer_id,void (*handler)(union sigval))
{
	struct 	sigevent sev;
	sev.sigev_notify = SIGEV_THREAD; //Upon timer expiration, invoke sigev_notify_function
	sev.sigev_notify_function = handler; //this function will be called when timer expires
	sev.sigev_notify_attributes = NULL;
	sev.sigev_value.sival_ptr = &timer_id;


	if(timer_create(CLOCK_REALTIME, &sev, timer_id) != 0) //on success, timer id is placed in timer_id
	{
		return ERROR;
	}  




    return SUCCESS;
}

/*****************************************************************
					Start configuration
			Parameter : delay in nano secs
*****************************************************************/
int kick_timer(timer_t timer_id, int interval_ns)
{
   struct itimerspec in;

	in.it_value.tv_sec = 0;
    in.it_value.tv_nsec = interval_ns; //sets initial time period
    in.it_interval.tv_sec = 0;
    in.it_interval.tv_nsec = interval_ns; //sets interval
    
    //issue the periodic timer request here.
    if( (timer_settime(timer_id, 0, &in, NULL)) != SUCCESS)
    {
    	return ERROR;
    }
    return SUCCESS;
}

/*****************************************************************
					Destroy Timer
*****************************************************************/
int stop_timer(timer_t timer_id)
{
	if( (timer_delete(timer_id)) != SUCCESS)
    {
    	printf("Error on delete timer function\n");
    	return ERROR;
    }
	

    return SUCCESS;
}
