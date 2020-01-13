#include "book.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char prefix[] ="books/";
char str[1<<8];
Book* OpenBook(int id) {
  sprintf(str, "/task2_book%d", id);
  int fd = shm_open(str, O_RDWR, 0666);
  if (fd < 0) return NULL;

  Book* self = mmap(NULL, sizeof(Book), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (self == MAP_FAILED) perror("OpenBook:First");
  int size = self->size;
  munmap(self, sizeof(Book));
  self = mmap(NULL, sizeof(Book) + size*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (self == MAP_FAILED) perror("OpenBook");

  return self;
}
Book* CreateBook(int id, int size) {
  sprintf(str, "/task2_book%d", id);

  int fd = shm_open(str, O_CREAT | O_RDWR, 0666);
  if (fd < 0)
    perror("CreateBook:shm_open");
  if (ftruncate(fd, sizeof(Book) + size*2))
    perror("CreateBook:ftruncate");

  Book* self = mmap(NULL, sizeof(Book) + size*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (self == MAP_FAILED) perror("CreateBook");

  InitBook(self, id, size);

  return self;
}
void InitBook(Book* self, int id, int size) {
  self->id = id;
  self->size = size;
  self->filled = 0;

  char* data = (char*)(self+1);
  char* written = data + size;
  memset(data, 0xff, size);
  memset(written, 0, size);
}
void DestroyBook(int id) {
  sprintf(str, "/task2_book%d", id);
  shm_unlink(str);
}
void PrintBook(Book* self) {
  if (!self) return;
  printf("Book%3d: ", self->id);
  if (self->size)
    printf("%6d/%6d %.2lf%%", self->filled, self->size, (double)self->filled/self->size*100);
  else printf("zero length");
  putchar('\n');
}
void SaveBook(Book* self) {
  char path[1<<8];
  sprintf(path, "%sbook%03d", prefix, self->id);
  #ifdef DBG
  printf("Writing %s\n", path);
  #endif
  FILE* f = fopen(path, "w");
  fwrite(self+1, 1, self->size, f);
  fclose(f);
}
int MasteredBook(Book* self) {
  if (!self) return 0;
  return (double)self->filled / self->size >= 0.85;
}