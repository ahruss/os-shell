#include "all.h"
#include <string.h>
#include <stdio.h>

void yyerror(char const* s) { fprintf(stderr, "%s\n", s); }
int yyparse(void);

/** 
 Called when we get EOF in yyparse
 */
int yywrap() { return 0; }

int main(int argc, const char * argv[]) {
    initShell();
    while (true) {
        char* prompt = getPrompt();
        if (yyparse()) {

        }
        if (lastShellError != NULL) {
            fprintf(stderr, "Error at line %d: %s\n", lineNumber,  lastShellError);
        }

        lastShellError = NULL;
        free(prompt);
        lineNumber++;
    }
}

