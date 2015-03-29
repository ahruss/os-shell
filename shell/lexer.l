%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "y.tab.h"
%}
%%
([^\<\>\|\"\\\& \t\n]+|\\.)+       yylval = strdup(yytext); return WORD;
\"                          yylval = strdup(yytext); return QUOTE;
\n                          return END_OF_STATEMENT;
[ \t]                       /* ignored */
\|                          return PIPE;
%%