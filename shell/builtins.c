#include "builtins.h"
#include <stdio.h>

bool isBuiltin(char* command) {
    return false;
}

int executeBuiltin(char* command, StringList* args) {
    // TODO: implement builtins
    printf("Executing builtin: %s, args %p", command, args);
    return 0;
}