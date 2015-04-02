%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include <readline/readline.h>
#include "y.tab.h"

#define HAVE_LIBREADLINE
#define HAVE_READLINE_HISTORY

//int readInput(char *buf, int size);
//#undef YY_INPUT
//#define YY_INPUT(buf,result,max_size) result = readInput(buf, max_size);

int yyparse(void);
char* getPrompt();
%}
%%
([^\<\>\|\"\\\& \t\n(\$\{)\}]+|\\.)+       yylval = strdup(yytext); return WORD;
\"                                 yylval = strdup(yytext); return QUOTE;
[ \t]                              yylval = strdup(yytext);return WHITESPACE;
2\>\&1                             yylval = strdup(yytext);return ERR_EQUALS_OUT;
2\>                                yylval = strdup(yytext);return ERROR_REDIRECT;
\|                                 yylval = strdup(yytext);return PIPE;
\n                                 yylval = strdup(yytext);return END_OF_STATEMENT;
\<                                 yylval = strdup(yytext);return LEFT_ARROW;
\>                                 yylval = strdup(yytext);return RIGHT_ARROW;
\&                                 yylval = strdup(yytext);return BACKGROUND;
\$\{                               yylval = strdup(yytext);return OPEN_VARIABLE;
\}                                 yylval = strdup(yytext);return CLOSE_VARIABLE;
%%

int readInput(char *buf, int size) {
    char *line;
    char* prompt = getPrompt();
    line = readline(prompt);
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
    strcat(buf, "\n");
    free(line);
    add_history(buf);
    return strlen(buf);
}
