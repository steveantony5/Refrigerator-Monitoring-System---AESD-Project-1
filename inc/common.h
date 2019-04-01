#ifndef COMMON_H
#define COMMON_H

/**
 * @\file	common.h
 * @\author	Steve Antony X
 * @\brief	This files contains the variables to be included in all the files
 * @\date	03/30/2019
 *
 */
/*****************************************************************
						Macros
*****************************************************************/
/*enabled as 1 if debug messages to be printed*/
#define DEBUG (1)

/*enabled as 1 if application to be built*/
#define NO_UNIT_TEST (1)

#define MAX_SOURCE_SIZE (50)

/*error codes and status*/
enum Status{SUCCESS = 0, ERROR = -1, LUX_ERROR = -2, REMOTE_SOCKET_ERROR = -3, LOGGER_ERROR = -4,  TEMP_ERROR = -1000, BRIGHT = 1000, DARK = -1000};


enum Status fridge_state;

char source_id_buffer[MAX_SOURCE_SIZE];


#endif