#include "i2c.h"

char bus[15];


int i2c_setup(int desp, int bus_no,int address)
{
	memset(bus,0,sizeof(bus));

	sprintf(bus,"/dev/i2c-%d",bus_no);

	if((desp = open(bus, O_RDWR)) < 0)
	{
		perror("Error on opening the bus\n");
		return 1;
	}

	if ((ioctl(desp, I2C_SLAVE, address)) < 0)
	{
		perror("Error on input output control of bus\n");
		return 1;
	}

	return 0;

}