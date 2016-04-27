#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) {
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    char* input = readline("liil> ");
    add_history(input);
    printf(input);
    free(input);
  }

  return 0;
}
