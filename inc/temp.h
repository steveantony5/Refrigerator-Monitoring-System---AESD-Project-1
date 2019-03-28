#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>

#include "lux.h"


#define TEMP_ADDR 0x48

typedef enum pointer_reg{
	tempReg,
	configReg,
	tlowReg,
	thighReg
}pointer_reg;


int file_des;
char bus[15];

//int i2c_setup(int bus_no,int address);

int temp_sensor_init();

int pointer_reg_write(pointer_reg reg);

int tlow_reg_read();

int thigh_reg_read();

int temp_read();

float temp_in_celcius();