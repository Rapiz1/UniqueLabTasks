CC = gcc
OPT = -Wall -g -lrt -pthread
.PHONEY = all

all: task1 task2 task3

read_data.o: read_data.h read_data.c
	$(CC) read_data.c -c -o $@ $(OPT)
book.o: book.h book.c
	$(CC) book.c -c -o $@ $(OPT)

task1: task1.c
	$(CC) $^ -o $@ $(OPT)
task2: task2.c read_data.o book.o
	$(CC) task2.c read_data.o book.o -o $@ $(OPT)
task3: task3.c
	$(CC) $^ -o $@ $(OPT)
clean:
	rm books/book*
	rm *.o
	rm task1 task2 task3
