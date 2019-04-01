CC = gcc
CFLAGS = -Wall -Werror -Iinc/ -pthread -lrt -g -lm

vpath %.c src/
SRC :=  i2c.c logger.c heartbeat.c POSIX_timer.c lux.c temp.c gpio.c remote_request.c lux_wrapper.c POST.c

OBJ :=$(SRC:.c=.o)
OP = main
INCLUDES := -Iinc/


CLIENT_SRC := client_socket.c
CLIENT_OBJ := client_socket.o
CLIENT_OP = client_socket
CLIENT_FLAGS = -Wall -Werror

LOG_FOLDER = ./log_folder

# For Building Application in Linux
all: $(OBJ)
	$(CC) -o $(OP) $(OBJ) $(CFLAGS) 

# For Building Remote client in Linux
client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) $(INCLUDES)  $(CLIENT_FLAGS) -o $(CLIENT_OP)

# For Building Application in BBG
bbg:
	arm-linux-gcc -o main src/POST.c src/i2c.c src/heartbeat.c src/logger.c src/POSIX_timer.c src/lux.c src/temp.c src/gpio.c src/remote_request.c src/lux_wrapper.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm

# For Building Application in BBG
bbg_remote_api:
	arm-linux-gcc -o remote_api src/client_socket.c -Wall -Werror

# For Building Unit test in Linux
test_linux:
	gcc -o test unit/unit.c  src/POST.c src/gpio.c src/logger.c src/heartbeat.c src/remote_request.c src/POSIX_timer.c src/i2c.c src/lux.c src/temp.c src/lux_wrapper.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm

# For Building Unit test in BBG
test_bbg:
	arm-linux-gcc -o test unit/unit.c  src/post.c src/gpio.c src/logger.c src/heartbeat.c src/remote_request.c src/POSIX_timer.c src/i2c.c src/lux.c src/temp.c src/lux_wrapper.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm

clean:
	rm -f *.o main remote_api client_socket test
	rm -rf $(LOG_FOLDER)
