#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
void cat(FILE* f) {
  char ch;
  while ((ch = fgetc(f)) != EOF) putchar(ch);
}
int main(int argc, char** argv) {
  puts("*rsh version*");
  if (argc == 1) {
    cat(stdin);
  } else {
    for (int i = 1; i < argc; i++) {
      FILE* f = fopen(argv[i], "r");
      if (f) {
        cat(f);
        fclose(f);
      } else {
        fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));
      }
    }
  }
  return 0;
}