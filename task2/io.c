#include "io.h"
#include "constants.h"

#include "stdio.h"
#include "stdlib.h"
FILE *stream[3];
char *line_buffer;
int position, buffer_size;
void InitLineBuffer() {
  line_buffer = malloc(sizeof(SHELL_BUFFER_LEN));
  buffer_size = SHELL_BUFFER_LEN;
  position = 0;
}
void Flush(int stream_id) {
  fprintf(stream[stream_id], "%s", line_buffer);
  free(line_buffer);
  InitLineBuffer();
}
void Print(char* str) {
  while (*str) {
    line_buffer[position++] = *str;
    if (*str == '\n')
      Flush(SHELL_STREAM_OUT);
    if (position == buffer_size) {
      buffer_size += SHELL_BUFFER_LEN;
      line_buffer = realloc(line_buffer, buffer_size);
    }
    str++;
  }
  line_buffer[position] = 0;
}
void PrintError(char* str) {
  fprintf(stream[SHELL_STREAM_ERR], "%s", str);
}