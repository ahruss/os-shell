#include "all.h"
#include <string.h>
#include <stdio.h>
#include "colors.h"

void yyerror(char const* s) { fprintf(stderr, "%s at line %d\n", s, lineNumber); }
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
            fprintf(stderr, "%sError at line %d: %s%s\n", RED, lineNumber,  lastShellError, RESET);
        }

        lastShellError = NULL;
        free(prompt);
        lineNumber++;
    }
}

