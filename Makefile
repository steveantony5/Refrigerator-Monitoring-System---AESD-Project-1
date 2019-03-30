CC = gcc
CFLAGS = -Wall -Werror -Iinc/ -pthread -lrt -g -lm

vpath %.c src/
SRC :=  i2c.c logger.c heartbeat.c POSIX_timer.c lux.c temp.c

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

bbg:
	arm-linux-gcc -o main src/i2c.c src/heartbeat.c src/logger.c src/POSIX_timer.c src/lux.c src/temp.c src/led.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm

bbg_remote_api:
	arm-linux-gcc -o remote_api src/client_socket.c -Wall -Werror

clean:
	rm -f *.o main
	rm -rf $(LOG_FOLDER)
