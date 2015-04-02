#include "all.h"
#include <string.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

void yyerror(char const* s) { fprintf(stderr, "%s\n", s); }
int yyparse(void);

/** 
 Called when we get EOF in yyparse
 */
int yywrap() { /*exitShell();*/ return 0; }

int main(int argc, const char * argv[]) {
    initShell();
    while (true) {
        char* prompt = getPrompt();
        if (yyparse()) {
            perror("parse");
        }
        if (lastShellError != NULL) perror(lastShellError);
        lastShellError = NULL;
        free(prompt);
    }
}

