#include "echo.h"
#include "constants.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

int Echo(char** args) {
  for (int i = 0; args[i]; i++) {
    if (i) Print(" ");
    Print(args[i]);
  }
  Print("\n");
  return EXIT_SUCCESS;
}