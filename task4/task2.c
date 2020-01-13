#include "read_data.h"
#include "book.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define PROCESS_NUM (1<<5)
const char path[] = "/dev/quantum_reader_1";

typedef struct {
  int books_left;
  sem_t mutex[MAX_BOOKS];
} Shared;

Shared* shared;

int InitShared() {
  shared = mmap(NULL, sizeof(Shared), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  shared->books_left = MAX_BOOKS;

  for (int i = 0; i < MAX_BOOKS; i++)
    if (sem_init(shared->mutex + i, 1, 1)) {
      perror("sem_init");
    }

  return 0;
}
void DestroyShared() {
  munmap(shared, sizeof(Shared));
  for (int i = 0; i < MAX_BOOKS; i++) DestroyBook(i);
}

void FillBook(Book* self, ReadData* data) {
  #ifdef DBG
  printf("Got Book%3d...\n", data->book_id);
  PrintReadData(data);
  #endif

  if (MasteredBook(self)) {
    #ifdef DBG
    printf("Skipping book%3d\n", self->id);
    #endif
  }
  else {
    self->id = data->book_id;
    self->size = data->book_size;

    int offset = data->data_offset;
    char* self_data = (char*)(self + 1);
    char* self_written = self_data + self->size;
    for (int i = 0; i < data->data_size; i++) {
      if (!self_written[i + offset]) {
        self->filled++;
        self_written[i + offset] = 1;
        self_data[i + offset] = data->data[i];
      }
    }

    if (MasteredBook(self)) {
      #ifdef DBG
      printf("Book%3d mastered\n", self->id);
      #endif
      shared->books_left--;
    }
  }
}
void PrintResult() {
  for (int i = 0; i < MAX_BOOKS; i++) {
    Book* book = OpenBook(i);
    if (!book) printf("Missing\n");
    else PrintBook(book);
  }
}
void SaveAll() {
  for (int i = 0; i < MAX_BOOKS; i++) {
    Book* book = OpenBook(i);
    if (book) SaveBook(book);
  }
}
int Runner() {
  while (shared->books_left) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
      perror("Runner");
      return 1;
    }
    ReadData *data = malloc(sizeof(ReadData) + MAX_DATA);
    read(fd, data, sizeof(ReadData) + MAX_DATA);

    int id = data->book_id, size = data->book_size;
    if (sem_wait(shared->mutex + id)) perror("wait");
    Book* book = OpenBook(id);
    if (!book) book = CreateBook(id, size);

    FillBook(book, data);

    PrintBook(book);
    if (sem_post(shared->mutex + id)) perror("post");
    close(fd);
  }
  return 0;
}

void CreateProcess() {
  pid_t pid = fork();  
  if (pid < 0) {
    perror("CreateProcess");
  }
  else if (pid == 0) {
    int status = Runner();
    #ifdef DBG
    printf("Subprocess exiting with %d\n", status);
    #endif
    exit(status);
  }
}

int main() {
  if (InitShared()) {
    fprintf(stderr, "Aborted\n");
    return 0;
  }
  for (int i = 0; i < PROCESS_NUM; i++) CreateProcess();
  while (wait(NULL) > 0);
  PrintResult();
  SaveAll();
  DestroyShared();  
}