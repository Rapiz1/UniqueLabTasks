#include "constants.h"
#include "shell.h"
#include "echo.h"
#include "io.h"
#include "helper.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

extern FILE* stream[3];
void ShInit() {
  InitLineBuffer();
  stream[0] = stdin;
  stream[1] = stdout;
  stream[2] = stdout;
}
void ShLoop() {
  char *line;
  char **args;
  int status;
  ShInit();

  do {
    line = ShReadLine(stdin);
    args = ShSplitLine(line);
    status = ShExecute(args);

    free(line);
    free(args);
  } while (status == EXIT_SUCCESS);

}
char* ShReadLine(FILE* in)
{
  char *line = NULL;
  line = readline("$ ");
  return line;
}
char** ShSplitLine(char* line) {
  int bufsize = SHELL_BUFFER_LEN, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "sh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SHELL_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SHELL_BUFFER_LEN;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SHELL_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
int ShExecute(char** args) {
  if (!args || !args[0])
    return EXIT_FAILURE;
  int status = 0;
  if (!strcmp(args[0], "echo"))
    status = Echo(args + 1);
  else if (!strcmp(args[0], "exit"))
    exit(EXIT_SUCCESS);
  else {
    status = ShLaunch(args);
  }
  return status;
}
int ShLaunch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = Fork();
  if (pid == 0) {
    // Child process
    status = EXIT_SUCCESS;
    if (execvp(args[0], args) == -1) {
      UnixError("sh");
      status = EXIT_FAILURE;
    }
    exit(status);
  } else if (pid < 0) {
    // Error forking
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return status;
}