#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1


#define NO_UNIT_TEST 0

#define MAX_SOURCE_SIZE 40

enum Status{SUCCESS = 0, ERROR = -1, LUX_ERROR = -2, REMOTE_SOCKET_ERROR = -3, LOGGER_ERROR = -4,  TEMP_ERROR = -1000, BRIGHT = 1000, DARK = -1000};


enum Status fridge_state;

char source_id_buffer[MAX_SOURCE_SIZE];


#endif