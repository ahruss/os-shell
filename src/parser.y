%{
#include <stdio.h>
#include <string.h>
#define YYSTYPE void*
#include "all.h"
int yydebug=1;

%}

%token WORD QUOTE END_OF_STATEMENT PIPE LEFT_ARROW RIGHT_ARROW ERR_EQUALS_OUT ERROR_REDIRECT BACKGROUND OPEN_VARIABLE CLOSE_VARIABLE WHITESPACE ERROR
%%

program         : execution
                        { return 0; }
                | empty
                        { return 0; }
                | program program
                ;

command         : word args
                        { $$ = newCommand($1, $2); }
                | word
                        { $$ = newCommand($1, NULL); }                                        ;

empty           : END_OF_STATEMENT
                ;

job             : command
                        { $$ = newJob(); pushCommand($1, $$ = newJob()); }
                | command PIPE job
                        { $$ = $3; pipeCommandTo($1, $$); }
                ;

execution       : job stdinRedirect stdoutRedirect stderrRedirect background END_OF_STATEMENT
                        { lastReturnCode = executeJob($1, $2, $3, $4, $5 != NULL); }
execution       : job stdinRedirect stdoutRedirect stderrRedirect background ERROR
                        { fprintf(stderr, "Unclosed variable expansion at line: %d.", lineNumber); lastReturnCode = 7; freeJob($1); }
                ;

stdinRedirect   :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   LEFT_ARROW word
                        { $$ = makeRedirect($2, RedirectTypeRead); }
                ;

stdoutRedirect  :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                |   RIGHT_ARROW word
                        { $$ = makeRedirect($2, RedirectTypeWrite); }
                |   RIGHT_ARROW RIGHT_ARROW word
                        { $$ = makeRedirect($3, RedirectTypeAppend); }                 ;

stderrRedirect   :
                        { $$ = makeRedirect(NULL, RedirectTypeNone); }
                 | ERR_EQUALS_OUT
                        { $$ = makeRedirect(NULL, RedirectTypeEquals); }
                 | ERROR_REDIRECT word
                        { $$ = makeRedirect($2, RedirectTypeWrite); }                 ;

background       :
                        { $$ = NULL; }
                 | whitespace BACKGROUND whitespace
                        { $$ = $1; } ;

args            : whitespace WORD whitespace
                        { $$ = newStringList($2); }
                | whitespace quotedString whitespace
                        { $$ = newStringList($2); ((StringList*)$$)->isQuoted = true; }
                | whitespace expandedVariable whitespace
                        { $$ = newStringList($2); }
                | args whitespace WORD whitespace
                        { $$ = listPush($1, $3); }
                | args whitespace quotedString whitespace
                        { $$ = listPush($1, $3); tailOf($$)->isQuoted = true; }
                | args whitespace expandedVariable whitespace
                        { $$ = listPush($1, $3); }
                ;

expandedVariable : OPEN_VARIABLE WORD CLOSE_VARIABLE
                        { $$ = expandVariable($2); };

word            : whitespace WORD whitespace { $$ = $2; }
                | whitespace quotedString whitespace { $$ = $2; }
                | whitespace expandedVariable whitespace { $$ = $2; }
| whitespace expandedVariable word whitespace
        {
            char* str = malloc(sizeof(char) * (strlen($2) + strlen($3) + 1));
            strcpy(str, $2);
            strcat(str, $3);
        }
                ;

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