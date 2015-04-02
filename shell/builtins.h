#ifndef __OSShell__builtins__
#define __OSShell__builtins__
#include "all.h"

int isBuiltin(char* command);

int set_env(char **args);
int printenv(char **args);
int unset_env(char **args);
int cd(char **args);
int alias(char **args);
int unalias(char **args);
int bye(char **args);

int executeBuiltin(char* command, StringList* args);
#endif /* defined(__OSShell__builtins__) */
