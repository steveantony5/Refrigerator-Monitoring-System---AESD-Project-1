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


socklen_t clilen;
struct sockaddr_in to_address;
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
                perror("Error opening server socket\n");
                return ERROR;
        }


        struct sockaddr_in server_address;

        memset(&server_address,0,sizeof(server_address));

        //assigning values for the server address structure
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port); // converting to network byte order
        server_address.sin_addr.s_addr = INADDR_ANY;



        if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0)
        {
            perror("Binding failed in the server");
            return ERROR;
        }

        /*Listening for clients*/
        if(listen(server_socket,LISTEN_MAX) < 0)
        {
            perror("Error on Listening ");
            return ERROR;
        }
        else
        {
            printf("\nlistening to remote requests.....\n");
        }

       
       
        

        
        return SUCCESS;

}

/**************************************************
       Function for remote request thread creation
       Parameters : Structure typecasted to void *
***************************************************/
void *remote_request_callback(void *arg)
{
	

	char buffer[MAX_BUFFER_SIZE];
    
    
    memset(buffer,'\0',MAX_BUFFER_SIZE);
    SOURCE_ID(source_id_buffer);
    sprintf(buffer,"DEBUG %s Remote request task started", source_id_buffer);
    mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);

    
	if(socket_creation_server(PORT_NO)== ERROR)
    {
        perror("Error on socket creation - killed remote request socket");
        memset(buffer,'\0',MAX_BUFFER_SIZE);
        SOURCE_ID(source_id_buffer);
        sprintf(buffer,"ERROR %s Socket for server failed- Killed remote request thread", source_id_buffer);
        mq_send(msg_queue, buffer, MAX_BUFFER_SIZE, 0);
        pthread_cancel(remote_request_thread);
    } 


    while(1)
    {
        new_socket = 0;
        memset(&to_address,0,sizeof(to_address));


        clilen = sizeof(to_address);

        new_socket = accept(server_socket,(struct sockaddr*) &to_address, &clilen);
        if(new_socket<0)
        {
            perror("Error on accepting client");
        }
        else
        {
            printf("established connection\n");
        }

    	int child_id = 0;
        /*Creating child processes*/
        /*Returns zero to child process if there is successful child creation*/
        child_id = fork();

        // error on child process
        if(child_id < 0)
        {
            perror("error on creating child\n");
            exit(1);
        }

        //closing the parent
        if (child_id > 0)
        {
            close(new_socket);
            waitpid(0, NULL, WNOHANG);  //Wait for state change of the child process
        }

        if(child_id == 0)
        {

            while(recv(new_socket, message ,MAX_BUFFER_SIZE, 0) > 0)
            {
            

                /*receives temperature request in celcius from remote request client*/
         	    if(strcmp(message,"tempc") == 0)
                {

                    memset(buffer,'\0',MAX_BUFFER_SIZE);
         		    printf("tempc Request sent to remote request\n");
                    float tempc = temp_in_celcius();
                    if((tempc != TEMP_ERROR) && (temp_dead_flag != 1))
                    {
                        sprintf(buffer,"Temperature in Celcius = %f\n",tempc);
                    }
                    else
                    {
                        sprintf(buffer,"Error on temperature sensor\n");
                    }

                    send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
         	    }

                /*receives temperature request in Kelvin from remote request client*/
                else if(strcmp(message,"tempk") == 0)
                {
                    memset(buffer,'\0',MAX_BUFFER_SIZE);
                    printf("tempk Request sent to remote request\n");
                    float tempk = temp_in_kelvin();
                    if((tempk != TEMP_ERROR) && (temp_dead_flag != 1))
                    {
                        sprintf(buffer,"Temperature in Kelvin = %f\n",tempk);
                    }
                    else
                    {
                        sprintf(buffer,"Error on temperature sensor\n");
                    }

                    send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
                }

                /*receives temperature request in fahrenheit from remote request client*/
                else if(strcmp(message,"tempf") == 0)
                {
                    memset(buffer,'\0',MAX_BUFFER_SIZE);
                    printf("tempf Request sent to remote request\n");
                    float tempf = temp_in_fahrenheit();
                    if((tempf != TEMP_ERROR) && (temp_dead_flag != 1))
                    {
                        sprintf(buffer,"Temperature in fahrenheit = %f\n",tempf);
                    }
                    else
                    {
                        sprintf(buffer,"Error on temperature sensor\n");
                    }


                    send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
                }

                /*receives brightness request in lux from remote request client*/
         	    else if(strcmp(message,"lux") == 0)
         	    {
                    memset(buffer,'\0',MAX_BUFFER_SIZE);
                    printf("tempk Request sent to remote request\n");
                    float lux = get_lux();
                    if((lux != ERROR) && (lux_dead_flag != 1))
                    {
                        sprintf(buffer,"brightness in lux = %f\n",lux);
                    }
                    else
                    {
                        sprintf(buffer,"Error on lux sensor\n");
                    }

                    send(new_socket, buffer, MAX_BUFFER_SIZE, 0);

         	    }

                /*receives fridge door status from remote request client*/
                else if(strcmp(message,"door") == 0)
                {
                    memset(buffer,'\0',MAX_BUFFER_SIZE);
                    printf("fridge state Request sent to remote request\n");
                    float lux = get_lux();
                    fridge_state = get_current_state_fridge(lux);
                    if((fridge_state == BRIGHT) && (lux_dead_flag != 1) && (lux != ERROR))
                    {
                        sprintf(buffer,"Fridge door is opened\n");
                    }
                    else if((fridge_state == DARK) && (lux_dead_flag != 1) && (lux != ERROR))
                    {
                        sprintf(buffer,"Fridge door is closed\n");
                    }
                    else
                    {
                        sprintf(buffer,"Fridge in unknown state\n");
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
            close(new_socket);
            exit(0); 
        }
    }   
}   