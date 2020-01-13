#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
const char path[] = "/dev/quantum_reader_0";
int main(int argc, char** argv) {
  if (argc == 1) {
    fprintf(stderr, "Characters count required\n");
    return 1;
  }
  int count = atoi(argv[1]);
  int fd = open(path, O_RDONLY);
  flock(fd, LOCK_EX);
  char* buf = malloc(sizeof(count));
  read(fd, buf, count);
  for (int i = 0; i < count; i++) putchar(buf[i]);
  free(buf);
  flock(fd, LOCK_UN);
  close(fd);
}