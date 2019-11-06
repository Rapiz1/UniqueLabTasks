// ATTENTION:
// only correctly handle SIGINT
// other unexpected terminated will break the setting of the terminal

#include "../constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>  //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>   //STDIN_FILENO
#include <signal.h>

struct termios oldt, newt;
struct winsize w;
int total_lines, pos;
int next(FILE* f, int num) {
  char buf[SHELL_BUFFER_LEN];
  int ret = 0;
  while (num-- > 0 && fgets(buf, SHELL_BUFFER_LEN, f)) {
    printf("%s", buf);
    ret++;
  }
  if (pos + ret < total_lines)
  printf("--More--(%d%%)\r", (int)((double)pos/total_lines*100 + 0.5));
  fflush(stdout);
  return ret;
}
void more(FILE* f) {
  total_lines = pos = 0;
  for (char ch; (ch = fgetc(f)) != -1;) total_lines += ch == '\n';
  fseek(f, 0, SEEK_SET);
  pos += next(f, w.ws_row);
  for (; pos < total_lines;) {
    char c = getchar();
    switch (c) {
      case 'f':
        pos += next(f, w.ws_row);
        break;
      case 's':
      case '\n':
      case 'z':
      case ' ':
        pos += next(f, 1);
        break;
      default:
        break;
    }
  }
}
void restore() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  putchar('\n');
  exit(0);
}
int main(int argc, char** argv) {
  puts("*rsh version*");

  /*tcgetattr gets the parameters of the current terminal
  STDIN_FILENO will tell tcgetattr that it should write the settings
  of stdin to oldt*/
  tcgetattr(STDIN_FILENO, &oldt);
  /*now the settings will be copied*/
  newt = oldt;

  /*ICANON normally takes care that one line at a time will be processed
  that means it will return if it sees a "\n" or an EOF or an EOL*/
  newt.c_lflag &= ~(ICANON | ECHO);

  /*Those new settings will be set to STDIN
  TCSANOW tells tcsetattr to change attributes immediately. */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  /*This is your part:
  I choose 'e' to end input. Notice that EOF is also turned off
  in the non-canonical mode*/

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  signal(SIGINT, restore);
  if (argc == 1)
    more(stdin);
  else
    for (int i = 1; i < argc; i++) {
      if (access(argv[i], F_OK)) {
        fprintf(stderr, "%s does not exist\n", argv[i]);
        continue;
      }
      FILE* f = fopen(argv[i], "r");
      if (i > 1) puts("************");
      more(f);
      fclose(f);
    }
  /*restore the old settings*/
  restore();
  return 0;
}