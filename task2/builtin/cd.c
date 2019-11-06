#include "../io.h"

#include "cd.h"
#include <unistd.h>
int Cd(char** args) {
  if (!args[1]) {
    PrintError("missing arguments\n");
    return 1;
  }
  return chdir(args[1]);
}