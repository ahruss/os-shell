#ifndef __OSShell__builtins__
#define __OSShell__builtins__
#include "all.h"

int isBuiltin(char* command);

int executeBuiltin(char* command, StringList* args, int commandIndex);

/**
 * Takes a username as input and retursn the full path to their home directory.
 */
char* getDirectoryFromUser(char*);
#endif /* defined(__OSShell__builtins__) */
