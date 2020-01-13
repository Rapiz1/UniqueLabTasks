#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THREADS 256
#define MAX_BOOKS 130
#define MAX_LENGTH 1000000
const char prefix[] = "books/";

pthread_t threads[MAX_THREADS];

pthread_rwlock_t rwlock[MAX_BOOKS];
char* content[MAX_BOOKS];
int size[MAX_BOOKS], final_size[MAX_BOOKS], ori_size[MAX_BOOKS];

int randint(int l, int r) {
  return rand()%(r - l + 1) + l;
}
void ShowPercent(int id) {
  printf("Book%03d: %d/%d(%.2f%%)\n", id, size[id], ori_size[id], (double)size[id]*100/ori_size[id]);
}
void* ShrinkBook(void* args) {
  int id = *(int*)args;
  char path[1<<8];
  sprintf(path, "%sbook%03d", prefix, id);
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror(path);
    return NULL;
  }
  char c[MAX_LENGTH];
  int len = read(fd, c, MAX_LENGTH);
  ori_size[id] = len;
  final_size[id] = len*0.2;

  // shuffle 0/1 array
  int lim = len/10;
  char s[MAX_LENGTH];
  for (int i = 0; i < len; i++)
    s[i] = i+1 <= lim;
  for (int i = 0; i < len; i++) {
    int j = randint(i, len - 1);
    char t = s[i]; s[i] = s[j]; s[j] = t;
  }

  char* c2 = malloc(lim);
  int len2 = 0;
  for (int i = 0; i < len; i++)
    if (s[i]) c2[len2++] = c[i];

  content[id] = c2;
  size[id] = len2;
  close(fd);
  printf("Shrinked book%03d\n", id);
  return NULL;
}
const struct timespec TIMEOUT = {0, 500};
void* EnlargeBook(void* args) {
  int id = *(int*)args;
  while (size[id] < final_size[id]) {
    int lhs = id, rhs = randint(0, MAX_BOOKS - 1);
    while (lhs == rhs) rhs = randint(0, MAX_BOOKS - 1);  

    if (pthread_rwlock_timedrdlock(rwlock + rhs, &TIMEOUT)) {
      continue;
    }
    if (pthread_rwlock_timedwrlock(rwlock + lhs, &TIMEOUT)) {
      pthread_rwlock_unlock(rwlock + rhs);
      continue;
    }
    int l = randint(0, size[rhs]-1), r = randint(l, size[rhs] - 1);
    int clip_size = r - l + 1;
    if (size[lhs] + clip_size > final_size[lhs]) {
      clip_size = final_size[lhs] - size[lhs];
      r = l + clip_size - 1;
    }
    printf("Enlarging book%03d += %d\n", lhs, clip_size);
    int new_size = size[lhs] + clip_size;
    int pos = randint(0, size[lhs]-1);

    content[lhs] = realloc(content[lhs], new_size);
    for (int i = 0; i < size[lhs] - pos; i++)
      content[lhs][new_size - 1 - i] = content[lhs][size[lhs] - 1 - i];
    for (int i = 0; i < clip_size; i++)
      content[lhs][pos + i] = content[rhs][l + i];

    size[id] = new_size;

    ShowPercent(lhs);
    pthread_rwlock_unlock(rwlock + lhs);
    pthread_rwlock_unlock(rwlock + rhs);
  }
  return NULL;
}
void* HandinBook(void* args) {
  int id = *(int*)args;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  char host[] = "192.168.0.105";
  //char host[] = "222.20.101.2";
  if (getaddrinfo(host, "2333", &hints, &res)) {
    fprintf(stderr, "HandinBook: failed getaddrinfo()");
    return NULL;
  }
  int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  //bind(sd, res->ai_addr, res->ai_addrlen);
  if (connect(sd, res->ai_addr, res->ai_addrlen)) {
    perror("Handinbook: connect");
    return NULL;
  }
  if (send(sd, content[id], size[id], 0) != size[id]) {
    fprintf(stderr, "Handinbook: partial sent\n");
  }
  close(sd);
  return NULL;
}

void SpinThread(void*(*func)(void*)) {
  int args[MAX_BOOKS];
  for (int i = 0; i < MAX_BOOKS; i++) {
    args[i] = i;
    pthread_create(threads + i, NULL, func, args + i);
  }
  for (int i = 0; i < MAX_BOOKS; i++)
    pthread_join(threads[i], NULL);
}
void Init() {
  srand(time(NULL));
  for (int i = 0; i < MAX_BOOKS; i++)
    pthread_rwlock_init(rwlock + i, NULL);
}
int main() {
  Init();

  SpinThread(ShrinkBook);
  puts("Shrinked all books");
  SpinThread(EnlargeBook); 
  SpinThread(HandinBook);
}