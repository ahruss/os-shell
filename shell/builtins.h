#ifndef __OSShell__builtins__
#define __OSShell__builtins__
#include "all.h"

int isBuiltin(char* command);

int setenv();
int printenv();
int unsetenv();
int cd();
int alias();
int unalias();
int bye();

int executeBuiltin(char* command, StringList* args);
#endif /* defined(__OSShell__builtins__) */