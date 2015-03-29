%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include <readline/readline.h>
#include "y.tab.h"

#define HAVE_LIBREADLINE
#define HAVE_READLINE_HISTORY

int readInput(char *buf, int size);
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result = readInput(buf, max_size);

int yyparse(void);
char* getPrompt();
%}
%%
([^\<\>\|\"\\\& \t\n]+|\\.)+       yylval = strdup(yytext); return WORD;
\"                          yylval = strdup(yytext); return QUOTE;
[ \t]                       /* ignored */
\|                          return PIPE;
\n                          return END_OF_STATEMENT;
%%

int readInput(char *buf, int size) {
    char *line;
    char* prompt = getPrompt();
    line = readline(prompt);
    free(prompt);
    if (!line) {
        fprintf(stderr, "no line");
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
