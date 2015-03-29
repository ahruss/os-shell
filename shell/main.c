#include "all.h"
#include <stdio.h>

void yyerror(char const* s) { fprintf(stderr, "%s\n", s); }

/** 
 Called when we get EOF in yyparse
 */
int yywrap() { exitShell(); return 0; }

int main(int argc, const char * argv[]) {
    while (true) {
        
        prompt();
        if (yyparse()) { }
        if (lastShellError != NULL) perror(lastShellError);
        lastShellError = NULL;
    }
}
