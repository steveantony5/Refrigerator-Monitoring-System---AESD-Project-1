CC = gcc
Flags = -Wall -Werror -pthread -lrt
SRC := logger.c main.c remote_request.c
OBJ :=$(SRC:.c=.o)
OP = main

CLIENT_SRC := client_socket.c
CLIENT_OBJ := client_socket.o
CLIENT_OP = client_socket
CLIENT_FLAGS = -Wall -Werror

LOG_FOLDER = ./log_folder
all: $(OBJ)
	$(CC) $(OBJ) $(Flags) -o $(OP)

client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) $(CLIENT_FLAGS) -o $(CLIENT_OP)


clean:
	rm $(OBJ) $(OP) $(CLIENT_OBJ) $(CLIENT_OP)
	rm -rf $(LOG_FOLDER)
