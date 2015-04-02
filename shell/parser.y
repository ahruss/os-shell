%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "all.h"
%}

%token WORD QUOTE END_OF_STATEMENT PIPE LEFT_ARROW RIGHT_ARROW ERR_EQUALS_OUT ERROR_REDIRECT BACKGROUND
%%

program         : execution               { return 0; }
                | empty                   { return 0; }                                                         ;

command         : word args               { $$ = newCommand($1, $2); }
                | word                    { $$ = newCommand($1, NULL); }                                        ;

empty           : END_OF_STATEMENT                                                                              ;

job             : command                 { $$ = newJob(); pushCommand($1, $$ = newJob()); }
                | command PIPE job        { $$ = $3; pipeCommandTo($1, $3); }                                   ;

execution       : job stdinRedirect stdoutRedirect stderrRedirect background END_OF_STATEMENT
                                          { lastReturnCode = executeJob($1, $2, $3, $4, $5 != NULL); }                  ;

stdinRedirect   :                         { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   LEFT_ARROW word       { $$ = makeRedirect($2, RedirectTypeRead); }                          ;

stdoutRedirect  :                                { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   RIGHT_ARROW word             { $$ = makeRedirect($2, RedirectTypeWrite); }
                |   RIGHT_ARROW RIGHT_ARROW word { $$ = makeRedirect($3, RedirectTypeAppend); }                 ;

stderrRedirect   :                                { $$ = makeRedirect(NULL, RedirectTypeNone); }
                 | ERR_EQUALS_OUT                 { $$ = makeRedirect(NULL, RedirectTypeEquals); }
                 | ERROR_REDIRECT word            { $$ = makeRedirect($2, RedirectTypeWrite); }                 ;

background       :                                { $$ = NULL; }
                 | BACKGROUND                     { $$ = $1; } ;

    args            : word                            { $$ = newStringList($1); }
                    | args word                       { $$ = listPush($1, $2); }                                            ;

word            : WORD
                | quotedString ;

quotePart       : WORD | LEFT_ARROW | RIGHT_ARROW | PIPE | ERR_EQUALS_OUT | ERROR_REDIRECT | BACKGROUND;
quoteParts      : quotePart                        { $$ = newStringList($1); }
                | quoteParts quotePart             { $$ = listPush($1, $2); }                                           ;

quotedString    : QUOTE quoteParts QUOTE       { $$ = joinWords($2); }                                                ;
%%