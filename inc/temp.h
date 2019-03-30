#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>


#include "i2c.h"


#define TEMP_ADDR 0x48

typedef enum pointer_reg{
	tempReg,
	configReg,
	tlowReg,
	thighReg
}pointer_reg;

#define SD_MODE (1 << 8)

// int file_des;

//int i2c_setup(int bus_no,int address);

int temp_sensor_init();

int pointer_reg_write(pointer_reg reg);

int tlow_reg_read();

int thigh_reg_read();

int temp_read();

float temp_in_celcius();

int tlow_reg_write(uint8_t);

int thigh_reg_write(uint8_t);

int config_reg_read();

int config_reg_write_default();

int config_read_conversion_rate();

int config_sd_continuous();

int config_sd();