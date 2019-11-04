CC = gcc -D_GNU_SOURCE -g -Wall
DIR = ../build/bin/
all: cat touch ls mkdir cp rm mv ln

ls: ls.c
	$(CC) ls.c -o $(DIR)ls
cat: cat.c
	$(CC) cat.c -o $(DIR)cat
touch: touch.c
	$(CC) touch.c -o $(DIR)touch
mkdir: mkdir.c
	$(CC) mkdir.c -o $(DIR)mkdir
cp: cp.c
	$(CC) cp.c -o $(DIR)cp
rm: rm.c
	$(CC) rm.c -o $(DIR)rm
mv: mv.c
	$(CC) mv.c -o $(DIR)mv
ln: ln.c
	$(CC) ln.c -o $(DIR)ln