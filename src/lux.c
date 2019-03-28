#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#define CONTROL_REGISTER (0X00)
#define TIMING_REGISTER (0X01)
#define DATA0LOW_REGISTER (0X0C)
#define DATA0HIGH_REGISTER (0X0D)
#define DATA1LOW_REGISTER (0X0E)
#define DATA1HIGH_REGISTER (0X0F)

#define WRITE_COMMAND (0x80)

int i2c_setup(int ,int );
int lux_sensor_setup();
int read_channel_0();
int read_channel_1();

int file_des;
char bus[15];
uint8_t register_data;
uint8_t MSB_0;
uint8_t LSB_0;

uint8_t MSB_1;
uint8_t LSB_1;

uint16_t CH0;
uint16_t CH1;

uint16_t ratio;


int main()
{
	if((i2c_setup(1,0x39)) != 0)
	{
		perror("Error on i2c bus set up for lux sensor");
		exit(1);
	}

	if(lux_sensor_setup()<0)
	{
		perror("Error on lux sensor configuration\n");
		exit(1);
	}

	while(1)
	{
		if(read_channel_0()<0)
		{
			perror("Error on reading channel 0\n");
			exit(1);
		}

		if(read_channel_1()<0)
		{
			perror("Error on reading channel 0\n");
			exit(1);
		}

		printf("CH0 %d\n",CH0);
		printf("CH1 %d\n",CH1);

		ratio = (CH1 / CH0);

		printf("ratio %d\n",ratio);

	}
}


int i2c_setup(int bus_no,int address)
{
	memset(bus,0,sizeof(bus));

	sprintf(bus,"/dev/i2c-%d",bus_no);

	if((file_des = open(bus, O_RDWR)) < 0)
	{
		perror("Error on opening the bus\n");
		return 1;
	}

	if ((ioctl(file_des, I2C_SLAVE, address)) < 0)
	{
		perror("Error on input output control of bus\n");
		return 1;
	}

	return 0;

}

int lux_sensor_setup()
{

	//command to write on control register
	register_data = WRITE_COMMAND | CONTROL_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//value for the control register
	register_data = 0x03;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//command to write on timing register
	register_data = WRITE_COMMAND | TIMING_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the control register\n");
		return -1;
	}

	//value for the control register
	register_data = 0x11;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the timing register\n");
		return -1;
	}

	return 0;

}

int read_channel_0()
{
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA0LOW_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des, &LSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0LOW_REGISTER register\n");
		return -1;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA0HIGH_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des, &MSB_0, 1) == -1)
	{
		perror("Error on reading the DATA0HIGH_REGISTER register\n");
		return -1;
	}

	CH0 = MSB_0 < 8;
	CH0 |= LSB_0;

	return 0;


}

int read_channel_1()
{
	//command to read on DATA0LOW register
	register_data = WRITE_COMMAND | DATA1LOW_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des, &LSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1LOW_REGISTER register\n");
		return -1;
	}

	//command to read on DATA0HIGH register
	register_data = WRITE_COMMAND | DATA1HIGH_REGISTER;

	if (write(file_des, &register_data, 1) == -1)
	{
		perror("Error on writing the command register\n");
		return -1;
	}


	if (read(file_des, &MSB_1, 1) == -1)
	{
		perror("Error on reading the DATA1HIGH_REGISTER register\n");
		return -1;
	}

	CH1 = MSB_1 < 8;
	CH1 |= LSB_1;

	return 0;

 
}


