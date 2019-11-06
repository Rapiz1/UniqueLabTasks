CC = gcc -D_GNU_SOURCE -g -Wall
DIR = ../build/bin/
DEPS = ../helper.c ../io.c

all: cat touch ls mkdir cp rm mv ln xargs

ls: ls.c
	$(CC) ls.c $(DEPS) -o $(DIR)ls
cat: cat.c
	$(CC) cat.c $(DEPS) -o $(DIR)cat
touch: touch.c
	$(CC) touch.c $(DEPS) -o $(DIR)touch
mkdir: mkdir.c
	$(CC) mkdir.c $(DEPS) -o $(DIR)mkdir
cp: cp.c
	$(CC) cp.c $(DEPS) -o $(DIR)cp
rm: rm.c
	$(CC) rm.c $(DEPS) -o $(DIR)rm
mv: mv.c
	$(CC) mv.c $(DEPS) -o $(DIR)mv
ln: ln.c
	$(CC) ln.c $(DEPS) -o $(DIR)ln
xargs: xargs.c
	$(CC) xargs.c $(DEPS) -o $(DIR)xargs