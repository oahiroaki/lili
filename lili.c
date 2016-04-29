#include <stdbool.h>

#include "mpc.h"

// Compiling on Windows
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// Fake readline function
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

// Fake add_history function
void add_history(char* unused) {}

// Otherwise include the editline headers
#else

#include <editline/readline.h>

#endif

long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  return 0;
}

long eval(mpc_ast_t* t) {
  // If tagged as number return it directly.
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  // The operator is always second child
  char* op = t->children[1]->contents;

  // We store the third child in `x`
  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char** argv) {
  // Create Some Parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lili = mpc_new("lili");

  // Define then with the following Language
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' | '%' ;            \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lili     : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lili);

  puts("Lili Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while (true) {
    char* input = readline("lili> ");
    add_history(input);

    // Attempt to parse the user input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lili, &r)) {
      mpc_ast_print(r.output);
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  // Undefine and delete our parsers
  mpc_cleanup(4, Number, Operator, Expr, Lili);

  return 0;
}