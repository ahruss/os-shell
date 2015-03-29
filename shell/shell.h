#ifndef OSShell_shell_h
#define OSShell_shell_h

#include <stdlib.h>
#include <sys/types.h>
#include "all.h"

/**
 The return code of the last executed job.
 */
extern int lastReturnCode;

/**
 The error code of the last error found.
 */
extern int lastErrorCode;
/**
 Saves the last error to be printed when available
 */
extern char* lastShellError;

/* need these to hide compiler warnings */
void yyerror(const char*);
int yylex();

/**
 Print out the prompt
 */
void prompt();

/**
 Clean up any required resources and exit the shell
 */
void exitShell();

/**
 Find the full path of an executable program given just the name 
 from somewhere in the path
 */
char* which(char* command);

/**
 Get the list of directories in the PATH
 */
StringList* getPathList();
#endif