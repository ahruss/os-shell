%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "all.h"
%}

%token WORD QUOTE END_OF_STATEMENT PIPE LEFT_ARROW RIGHT_ARROW ERR_EQUALS_OUT ERROR_REDIRECT BACKGROUND OPEN_VARIABLE CLOSE_VARIABLE WHITESPACE
%%

program         : execution
                        { return 0; }
                | empty
                        { return 0; }
                ;

command         : word whitespace args
                        { $$ = newCommand($1, $3); }
                | word
                        { $$ = newCommand($1, NULL); }                                        ;

empty           : whitespace END_OF_STATEMENT
                ;

job             : command
                        { $$ = newJob(); pushCommand($1, $$ = newJob()); }
                | command whitespace PIPE whitespace job
                        { $$ = $5; pipeCommandTo($1, $5); }
                ;

execution       : job whitespace stdinRedirect whitespace stdoutRedirect whitespace stderrRedirect whitespace background whitespace  END_OF_STATEMENT
                        { lastReturnCode = executeJob($1, $3, $5, $7, $9 != NULL); }
                ;

stdinRedirect   :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   LEFT_ARROW whitespace word
                        { $$ = makeRedirect($3, RedirectTypeRead); }
                ;

stdoutRedirect  :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   RIGHT_ARROW whitespace word
                        { $$ = makeRedirect($3, RedirectTypeWrite); }
                |   RIGHT_ARROW RIGHT_ARROW whitespace word
                        { $$ = makeRedirect($4, RedirectTypeAppend); }                 ;

stderrRedirect   :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                 | ERR_EQUALS_OUT
                        { $$ = makeRedirect(NULL, RedirectTypeEquals); }
                 | ERROR_REDIRECT whitespace word
                        { $$ = makeRedirect($3, RedirectTypeWrite); }                 ;

background       :
                        { $$ = NULL; }
                 | BACKGROUND
                        { $$ = $1; } ;

args            : word
                        { $$ = newStringList($1); }
                | args whitespace word
                        { $$ = listPush($1, $2); }
                ;

expandedVariable : OPEN_VARIABLE WORD CLOSE_VARIABLE
                        { $$ = expandVariable($2); };

word            : WORD
                | quotedString
                | expandedVariable;

whitespace      : | WHITESPACE | whitespace WHITESPACE ;

quotePart       : WORD | LEFT_ARROW | RIGHT_ARROW | PIPE | ERR_EQUALS_OUT
                | ERROR_REDIRECT | BACKGROUND | expandedVariable | OPEN_VARIABLE
                | CLOSE_VARIABLE | WHITESPACE
                ;
quoteParts      : quotePart
                        { $$ = newStringList($1); }
                | quoteParts quotePart
                        { $$ = listPush($1, $2); }
                ;

quotedString    : QUOTE quoteParts QUOTE
                        { $$ = joinWords($2); }
                ;
%%