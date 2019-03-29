// C program to implement one side of FIFO 
// This side writes first, then reads 
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
int main() 
{ 
    int fd; 
  
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
    char arr1[80], arr2[80]; 
    // Open FIFO for write only 
        fd = open(myfifo, O_WRONLY);
    while (1) 
    { 
         
  
        // Take an input arr2ing from user. 
        // 80 is maximum length 
        printf("Enter string\n");
        scanf("%s",arr2);
  
        // Write the input arr2ing on FIFO 
        // and close it 
        write(fd, arr2, 80); 
        memset(arr2,0,sizeof(arr2));


    } 
    return 0; 
}