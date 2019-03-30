#ifndef POSIX_Timer_H_
#define POSIX_Timer_H_
/*****************************************************************
						Includes
*****************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

/*****************************************************************
						Function Protypes
*****************************************************************/
int kick_timer(timer_t, int);
int setup_timer_POSIX(timer_t *,void (*handler)(union sigval));
int stop_timer(timer_t);
void temp_timer_handler(union sigval);
void lux_timer_handler(union sigval);
void log_timer_handler(union sigval);

/*****************************************************************
						Globals
*****************************************************************/
extern timer_t timer_id_temp, timer_id_lux, timer_id_heartbeat, timer_id_log;

extern int FLAG_READ_TEMP;
extern int FLAG_READ_LUX;
extern int FLAG_LOG;
/*****************************************************************
						MACROS
*****************************************************************/
#define Delay_NS (200000000)//200ms


#endif /* POSIX_Timer_H_ */