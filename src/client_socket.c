#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define PORT_NO 6000
#define LOOP_BACK_ADDR "127.0.0.1"
#define MAX_BUFFER_SIZE 100

char message[MAX_BUFFER_SIZE];
int client_socket;

/******************************************************
       Function for client socket creation
       Parameters : port number, ip address in string
*******************************************************/
int socket_creation_client(int port,char ip[20])
{
    client_socket = socket(AF_INET,SOCK_STREAM,0);// setting the client socket

    if(client_socket < 0 ) // enters this loop if port number is not given as command line argument
    {
        //printing error message when opening client socket
        printf("\nError opening client socket\n");
        return 1;
    }

    struct sockaddr_in server_address ;
    memset(&server_address,0,sizeof(server_address));
    //assigning values for the server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); // converting to network byte order
    server_address.sin_addr.s_addr = inet_addr(ip);


    if(connect(client_socket,(struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        printf("Error on connect\n");
        return 1;
    }

    return 0;

}


int main()
{
    if(socket_creation_client(PORT_NO,LOOP_BACK_ADDR) != 0)
    {
        return 1;
    }
    char buffer[MAX_BUFFER_SIZE];
    char message[MAX_BUFFER_SIZE];
    while(1)
    {
        printf("********************** Welcome ***************************\n");
        printf("1. To know current temperature in Celcius, Type \"tempc\"\n");
        printf("2. To know current temperature in Kelvin, Type \"tempk\"\n");
        printf("3. To know current temperature in Farenheit, Type \"tempf\"\n");
        printf("4. To know current brightness in lux, Type \"lux\"\n");
        printf("5. To know current fridge door state, Type \"door\"\n");
        printf("6. To exit, Press Ctrl+c\n");

        scanf("%[^\n]%*c", buffer);
        send(client_socket, buffer, MAX_BUFFER_SIZE , 0);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        recv(client_socket, message ,MAX_BUFFER_SIZE, 0);
        printf("%s\n", message);
        memset(message, 0, MAX_BUFFER_SIZE);
    }
}