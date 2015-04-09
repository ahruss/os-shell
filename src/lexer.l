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
([^\<\>\|\"\\\& \t\n]+|\\.)+             yylval = strdup(yytext);return WORD;
\"                                       yylval = strdup(yytext);return QUOTE;
[ \t]                                    yylval = strdup(yytext);return WHITESPACE;
2\>\&1                                   yylval = strdup(yytext);return ERR_EQUALS_OUT;
2\>                                      yylval = strdup(yytext);return ERROR_REDIRECT;
\|                                       yylval = strdup(yytext);return PIPE;
\n                                       yylval = strdup(yytext);return END_OF_STATEMENT;
\<                                       yylval = strdup(yytext);return LEFT_ARROW;
\>                                       yylval = strdup(yytext);return RIGHT_ARROW;
\&                                       yylval = strdup(yytext);return BACKGROUND;
%%

int readInput(char *buf, int size) {
    char *line = NULL;
    size_t lineSize = 0;
    char* prompt = getPrompt();

    #ifdef HAS_READLINE
        line = readline(prompt);
    #else
        printf("%s", prompt);
        getline(&line, &lineSize, yyin);
    #endif
    free(prompt);
    if (!line) {
        exitShell();
        return 0;
    }
    if (strlen(line) > size - 2) {
        fprintf(stderr, "input line too long");
        return 0;
    }
    strcpy(buf, line);
    #ifdef HAS_READLINE
        strcat(buf, "\n");
    #endif
    free(line);
    #ifdef HAS_READLINE
        add_history(buf);
    #endif
    return (int)strlen(buf);
}
