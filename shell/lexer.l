%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "y.tab.h"
%}
%%
[^\<\>\|\"\\\& \t\n]+       yylval = strdup(yytext); return WORD;
\"([^\"]+|\\\")*\"          yylval = strdup(yytext); return QUOTED_STRING;
\n                 return END_OF_STATEMENT;
[ \t]                       /* ignored */
\|                          return PIPE;
%%