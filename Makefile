CC = gcc
Flags = -Wall -Werror -pthread -lrt
SRC := logger.c main.c
OBJ :=$(SRC:.c=.o)
OP = main
LOG_FOLDER = ./log_folder
all: $(OBJ)
	$(CC) $(OBJ) $(Flags) -o $(OP)


clean:
	rm $(OBJ) $(OP)
	rm -rf $(LOG_FOLDER)
