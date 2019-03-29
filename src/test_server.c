// C program to implement one side of FIFO 
// This side reads first, then reads 
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h>
int main() 
{ 
    int fd1; 
  
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mkfifo(myfifo, 0666); 
  
    char str1[80], str2[80]; 
    // First open in read only and read 
        fd1 = open(myfifo,O_RDONLY|O_NONBLOCK);
    while (1) 
    { 
         
        read(fd1, str1, 80);
        if(strlen(str1) > 0)
        {
            // Print the read string and close 
            printf("User1: %s\n", str1); 
            memset(str1,0,sizeof(str1));
        }
        printf("do other work\n");

  
        
    } 
    return 0; 
} 