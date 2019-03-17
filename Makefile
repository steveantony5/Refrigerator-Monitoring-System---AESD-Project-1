CC = gcc
CFLAGS = -Wall -Werror -Iinc/ -pthread -lrt -g

vpath %.c src/
SRC := logger.c remote_request.c heartbeat.c POSIX_timer.c

OBJ :=$(SRC:.c=.o)
OP = main
INCLUDES := -Iinc/


CLIENT_SRC := client_socket.c
CLIENT_OBJ := client_socket.o
CLIENT_OP = client_socket
CLIENT_FLAGS = -Wall -Werror

LOG_FOLDER = ./log_folder

all: $(OBJ)
	$(CC) -o $(OP) $(OBJ) $(CFLAGS) 

client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) $(INCLUDES)  $(CLIENT_FLAGS) -o $(CLIENT_OP)


clean:
	rm $(OBJ) $(OP) $(CLIENT_OBJ) $(CLIENT_OP)
	rm -rf $(LOG_FOLDER)
