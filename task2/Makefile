OBJS = *.c builtin/*.c
CC = gcc
LINKER_FLAGS = -lreadline
COMPLIER_FLAGS = -g -D_GNU_SOURCE -Wall
OBJ_NAME = rsh

all: $(OBJS)
	mkdir -p build
	$(CC) $(OBJS) $(LINKER_FLAGS) $(COMPLIER_FLAGS) -o build/$(OBJ_NAME)
run: all
	build/$(OBJ_NAME)