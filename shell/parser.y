%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "all.h"
%}

%token WORD QUOTE END_OF_STATEMENT PIPE END_OF_PROGRAM
%%

program       : execution               { return 0; }
              | empty                   { return 0; }                               ;

command       : word args               { $$ = newCommand($1, $2); }
              | word                    { $$ = newCommand($1, NULL); }              ;

empty         : END_OF_STATEMENT ;

job           : command                 { $$ = newJob(); pushCommand($1, $$ = newJob()); }
              | command PIPE job        { $$ = $3; pipeCommandTo($1, $3); }         ;

execution     : job END_OF_STATEMENT    { lastReturnCode = executeJob($1); }             ;

args          : word                    { $$ = newStringList($1); }
              | args word               { $$ = listPush($1, $2); }                  ;

word          : WORD
              | quotedString ;

quotedString  : QUOTE args QUOTE       { $$ = joinWords($2); }                     ;

exit          : END_OF_PROGRAM          { exitShell(); }                            ;
%%