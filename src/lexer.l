%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "all.h"
#define YYSTYPE void*
#include "y.tab.h"
#include "shell.h"

int readInput(char *buf, int size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result = readInput(buf, max_size);

#ifdef HAS_READLINE
#include <readline/readline.h>
#endif


int yyparse(void);
char* getPrompt();
%}
%%
\$\{                                     yylval = strdup(yytext);return OPEN_VARIABLE;
\}                                       yylval = strdup(yytext);return CLOSE_VARIABLE;
([^\<\>\|\"\\\& \t\n\r]+|\\.)+             yylval = strdup(yytext);return WORD;
\"                                       yylval = strdup(yytext);return QUOTE;
[ \t]                                    yylval = strdup(yytext);return WHITESPACE;
2\>\&1                                   yylval = strdup(yytext);return ERR_EQUALS_OUT;
2\>                                      yylval = strdup(yytext);return ERROR_REDIRECT;
\|                                       yylval = strdup(yytext);return PIPE;
\n|\r                                    yylval = strdup(yytext);return END_OF_STATEMENT;
\<                                       yylval = strdup(yytext);return LEFT_ARROW;
\>                                       yylval = strdup(yytext);return RIGHT_ARROW;
\&                                       yylval = strdup(yytext);return BACKGROUND;
%%

int readInput(char *buf, int size) {
    char *line = NULL;
    char* prompt = getPrompt();
    printf(prompt);
    free(prompt);
    line = getNextLine();
    if (!line) {
        exitShell();
        return 0;
    }
    if (strlen(line) > size - 2) {
        fprintf(stderr, "input line too long");
        return 0;
    }
    strcpy(buf, line);
    strcat(buf, "\n");
    free(line);
    return (int)strlen(buf);
}
