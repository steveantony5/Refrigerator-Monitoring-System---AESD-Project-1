/*****************************************************************
                        Includes
*****************************************************************/
#include "remote_request.h"

/*****************************************************************
       Function for server socket creation
       Parameters : Port number
*****************************************************************/
int socket_creation_server(int port)
{

        //creating the socket for client 

        server_socket = socket(AF_INET,SOCK_STREAM,0);// setting the client socket
        if(server_socket < 0 ) // enters this loop if port number is not given as command line argument
        {
                //printing error message when opening client socket
                printf("\nError opening server socket\n");
                return 1;
        }


        struct sockaddr_in server_address;

        memset(&server_address,0,sizeof(server_address));

        //assigning values for the server address structure
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port); // converting to network byte order
        server_address.sin_addr.s_addr = INADDR_ANY;



        if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
        {
            printf("Binding failed in the server");
            return 1;
        }

        /*Listening for clients*/
        if(listen(server_socket,LISTEN_MAX) < 0)
        {
            printf("Error on Listening ");
            return 1;
        }
        else
        {
            printf("\nlistening.....\n");
        }

       
        /*Accepting Client connection*/
        new_socket = 0;
        socklen_t clilen;
        struct sockaddr_in to_address;
        memset(&to_address,0,sizeof(to_address));


        clilen = sizeof(to_address);

        new_socket = accept(server_socket,(struct sockaddr*) &to_address, &clilen);
        if(new_socket<0)
        {
            perror("Error on accepting client");
            return 1;
        }
        else
        {
            printf("established connection\n");
        }
        return 0;

}

/*****************************************************************
       Function for remote request thread creation
       Parameters : Structure typecasted to void *
*****************************************************************/
void *remote_request_callback(void *arg)
{
	

	char buffer[MAX_BUFFER_SIZE];
    char file_name[MAX_BUFFER_SIZE];
    
    file_descriptors *fd = (file_descriptors *)arg;

    sprintf(file_name,"%s%s/%s",LOG_PATH,fd->file_path,fd->file_name);

    sprintf(buffer,"%s","Remote request task started");

    
	if(socket_creation_server(PORT_NO)!=0)
    {
        return 0;
    } 

    LOG_MESSAGE(file_name,"%s %s %s\n", log_level[0], time_stamp(),buffer);

    while(1)
    {
    	 //receiving from the other process
         if(recv(new_socket, message ,MAX_BUFFER_SIZE, 0))
         {
         	LOG_MESSAGE(file_name,"%s %s %s\n", log_level[0], time_stamp(), message);
         	printf("Received: %s\n",message);

         	if(strcmp(message,"temp") == 0)
         	{
         		LOG_MESSAGE(file_name,"%s %s %f\n", log_level[0], time_stamp(), get_temperature());
         		printf("Temperature = %f\n", get_temperature());
         	}

         	else if(strcmp(message,"lux") == 0)
         	{
         		LOG_MESSAGE(file_name,"%s %s %f\n", log_level[0], time_stamp(), get_lux());
         		printf("Lux = %f\n", get_lux());
         	}
         }
    }
}