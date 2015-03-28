#ifndef __OSShell__builtins__
#define __OSShell__builtins__
#include "all.h"
bool isBuiltin(char* command);
int executeBuiltin(char* command, StringList* args);
#endif /* defined(__OSShell__builtins__) */