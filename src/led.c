#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>


int led_on()
{
	FILE *file_ptr = fopen("/sys/class/gpio/gpio49/value", "w");
	// if(file_ptr == -1)
	// {
	// 	perror("LED file open failed");
	// 	return -1;
	// }
	fputc('0', file_ptr);
	fclose(file_ptr);	

	return 0;
}

int led_off()
{
	FILE *file_ptr = fopen("/sys/class/gpio/gpio49/value", "w");
	
	fputc('1', file_ptr);
	fclose(file_ptr);	

	return 0;
}

int gpio_pin_init()
{
	FILE *file_ptr = fopen("/sys/class/gpio/export", "w");
	
	fprintf(file_ptr,"%d",49);
	fclose(file_ptr);

	file_ptr = fopen("/sys/class/gpio/gpio49/direction", "w+");

	fprintf(file_ptr,"out");
	fclose(file_ptr);

	return 0;	

}

// int main()
// {
// 	gpio_pin_init();
	
// 	while(1)
// 	{
// 		led_on();
// 		sleep(1);
		
// 		led_off();
// 		sleep(1);	
// 	}
	
// }