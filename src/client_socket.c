/*****************************************************************
                        Includes
*****************************************************************/
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define SIZE (20)
// FIFO file path 
char * myfifo_temp = "/tmp/fifo_temp"; 
char * myfifo_lux = "/tmp/fifo_lux"; 

int main() 
{ 
    int fd_temp, fd_lux; 
  
    

    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo_temp, 0666); 
    mkfifo(myfifo_lux, 0666); 

    // Open FIFO for write only 
    fd_temp = open(myfifo_temp, O_RDWR|O_NONBLOCK);
    fd_lux = open(myfifo_lux, O_RDWR|O_NONBLOCK);


    char command[SIZE]; 
    char buffer[SIZE];
    
    while (1) 
    { 
        printf("Menu:\n\tTemperature in Kelvin -> temp-k\n\tTemperature in Celcius -> temp-c\n\tTemperature in Farenheit -> temp-f\n\tLux value -> lux\n\n");
        printf("Enter your option : ");
        scanf("%s",command);
  
        printf("\n\n");

        if(strcmp(command,"temp-k") == 0)
        {
            write(fd_temp, command, SIZE);
            printf("request sent to temp in K\n");
            
            memset(buffer,0,SIZE);
            read(fd_temp, buffer, SIZE);
            printf("%s\n",buffer);
        }

        else if(strcmp(command,"temp-c") == 0)
        {
            write(fd_temp, command, SIZE);
            printf("request sent to temp in C\n");

            memset(buffer,0,SIZE);
            read(fd_temp, buffer, SIZE);
            printf("%s\n",buffer);
        }

        else if(strcmp(command,"temp-f") == 0)
        {
            write(fd_temp, command, SIZE);
            printf("request sent to temp in F\n");

            memset(buffer,0,SIZE);
            read(fd_temp, buffer, SIZE);
            printf("%s\n",buffer);
        }

        else if(strcmp(command,"lux") == 0)
        {
            write(fd_lux, command, SIZE);
            printf("request sent to lux\n");

            memset(buffer,0,SIZE);
            read(fd_temp, buffer, SIZE);
            printf("%s\n",buffer);

        }

    } 
    return 0; 
}

