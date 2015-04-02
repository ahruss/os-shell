#ifndef __OSShell__builtins__
#define __OSShell__builtins__
#include "all.h"

int isBuiltin(char* command);

int executeBuiltin(char* command, StringList* args, int commandIndex);
#endif /* defined(__OSShell__builtins__) */
