%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "all.h"
%}

%token WORD QUOTE END_OF_STATEMENT QUOTED_STRING PIPE
%%

program     :   execution
                    {   return 0; }
            |   empty
                    { return 0; }
            ;

command     :   WORD args
                    { $$ = newCommand($1, $2); }
            |   WORD
                    { $$ = newCommand($1, NULL); }
            ;

empty       :   END_OF_STATEMENT ;

job         :   command
                    {   $$ = newJob(); pushCommand($1, $$); }
            |   command PIPE job
                    { pipeCommandTo($1, $3); $$ = $3; }
            ;

execution   :   job END_OF_STATEMENT
                    { $$ = (void*)executeJob($1); }
            ;

args        :   arg
                    { $$ = newStringList($1); }
            |   args arg
                    { $$ = listPush($1, $2); }
            ;

arg         :   WORD
            |   QUOTED_STRING
            ;
%%
