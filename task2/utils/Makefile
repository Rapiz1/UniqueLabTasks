CC = gcc -D_GNU_SOURCE -g -Wall

all: cat touch ls mkdir cp rm mv ln

ls: ls.c
	$(CC) ls.c -o build/ls
cat: cat.c
	$(CC) cat.c -o build/cat
touch: touch.c
	$(CC) touch.c -o build/touch
mkdir: mkdir.c
	$(CC) mkdir.c -o build/mkdir
cp: cp.c
	$(CC) cp.c -o build/cp
rm: rm.c
	$(CC) rm.c -o build/rm
mv: mv.c
	$(CC) mv.c -o build/mv
ln: ln.c
	$(CC) ln.c -o build/ln