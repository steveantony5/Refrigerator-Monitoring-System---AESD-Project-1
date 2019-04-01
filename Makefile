CC = gcc
CFLAGS = -Wall -Werror -Iinc/ -pthread -lrt -g -lm

vpath %.c src/
SRC :=  i2c.c logger.c heartbeat.c POSIX_timer.c lux.c temp.c led.c remote_request.c lux_wrapper.c

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
	arm-linux-gcc -o main src/i2c.c src/heartbeat.c src/logger.c src/POSIX_timer.c src/lux.c src/temp.c src/led.c src/remote_request.c src/lux_wrapper.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm

bbg_remote_api:
	arm-linux-gcc -o remote_api src/client_socket.c -Wall -Werror

unit_test:
	arm-linux-gcc -o test unit/unit.c -Wall -Werror

test_linux:
	arm-linux-gcc -o test unit/unit.c  src/led.c src/logger.c src/heartbeat.c src/remote_request.c src/POSIX_timer.c src/i2c.c src/lux.c src/temp.c src/lux_wrapper.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm


test_temp:
	arm-linux-gcc -o temperature_unit unit/temperature_unit.c src/led.c src/i2c.c src/lux.c src/temp.c -Wall -Werror -Iinc/ -pthread -lrt -g -lm
clean:
	rm -f *.o main remote_api client_socket test
	rm -rf $(LOG_FOLDER)

uclean:
	rm -f test
