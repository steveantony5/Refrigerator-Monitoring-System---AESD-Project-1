/**
 * @\file   remote_request.c
 * @\author Sanju Prakash Kannioth
 * @\brief  This files contains the definitions of Socket IPC and remote request server theread
 * @\date   03/30/2019
 *
 */

/*****************************************************************
                        Includes
*****************************************************************/
#include "remote_request.h"


/**********************************************
       Function for server socket creation
       Parameters : Port number
***********************************************/
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

/**************************************************
       Function for remote request thread creation
       Parameters : Structure typecasted to void *
***************************************************/
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
         
    	 
         if(recv(new_socket, message ,MAX_BUFFER_SIZE, 0))
         {

            /*receives temperature request in celcius from remote request client*/
         	if(strcmp(message,"tempc") == 0)
         	{

                memset(buffer,'\0',MAX_BUFFER_SIZE);
         		printf("tempc Request sent to remote request\n");
                float tempc = temp_in_celcius();
                if(tempc == ERROR)
                {
                    sprintf(buffer,"Error on temperature sensor\n");
                }
                else
                {
                    sprintf(buffer,"Temperature in Celcius = %f\n",tempc);
                }

                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
         	}

            /*receives temperature request in Kelvin from remote request client*/
            else if(strcmp(message,"tempk") == 0)
            {
                memset(buffer,'\0',MAX_BUFFER_SIZE);
                printf("tempk Request sent to remote request\n");
                float tempk = temp_in_celcius();
                if(tempk == ERROR)
                {
                    sprintf(buffer,"Error on temperature sensor\n");
                }
                else
                {
                    sprintf(buffer,"Temperature in Kelvin = %f\n",(tempk+273.15));
                }

                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
            }

            /*receives temperature request in Farenheit from remote request client*/
            else if(strcmp(message,"tempf") == 0)
            {
                memset(buffer,'\0',MAX_BUFFER_SIZE);
                printf("tempf Request sent to remote request\n");
                float tempf = temp_in_celcius();
                if(tempf == ERROR)
                {
                    sprintf(buffer,"Error on temperature sensor\n");
                }
                else
                {
                    sprintf(buffer,"Temperature in Farenheit = %f\n",((tempf * 9/5) + 32));
                }

                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
            }

            /*receives brightness request in lux from remote request client*/
         	else if(strcmp(message,"lux") == 0)
         	{
                memset(buffer,'\0',MAX_BUFFER_SIZE);
                printf("tempk Request sent to remote request\n");
                float lux = get_lux();
                if(lux == ERROR)
                {
                    sprintf(buffer,"Error on lux sensor\n");
                }
                else
                {
                    sprintf(buffer,"brightness in lux = %f\n",lux);
                }

                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);

         	}

            /*receives fridge door status from remote request client*/
            else if(strcmp(message,"door") == 0)
            {
                memset(buffer,'\0',MAX_BUFFER_SIZE);
                printf("fridge state Request sent to remote request\n");
                fridge_state = get_current_state_fridge(get_lux());
                if(fridge_state == ERROR)
                {
                    sprintf(buffer,"Fridge in unknown state\n");
                }
                else if(fridge_state == BRIGHT)
                {
                    sprintf(buffer,"Fridge door is opened\n");
                }
                else if(fridge_state == DARK)
                {
                    sprintf(buffer,"Fridge door is Closed\n");
                }

                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);

            }

            /*Invalid remote request client*/
            else
            {
                memset(buffer,'\0',MAX_BUFFER_SIZE);
                printf("Invalid remote request\n");
                sprintf(buffer,"Invalid remote request\n");
                send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
            }
         }
    }
}