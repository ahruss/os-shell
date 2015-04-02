#include "builtins.h"
#include <stdio.h>

#define NUMBUILTINS 7

char* builtin_commands[] = {
    "setenv",
    "printenv",
    "unsetenv",
    "cd",
    "alias",
    "unalias",
    "bye"
};

int (*builtin_functions[]) (char **) = {
    &setenv,
    &printenv,
    &unsetenv,
    &cd,
    &alias,
    &unalias,
    &bye
};

int setenv() {
    return 1;
}

int printenv() {
    return 1;
}

int unsetenv() {
    return 1;
}

int cd() {
    return 1;
}

int alias() {
    return 1;
}

int unalias() {
    return 1;
}

int bye() {
    return 1;
}


int isBuiltin(char* command) {
    for(int i = 0; i < NUMBUILTINS; i++) {
        if(strcmp(command, builtin_commands[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int executeBuiltin(char* command, StringList* args) {
    // TODO: implement builtins
    printf("Executing builtin: %s, args %p", command, args);
    return 0;
}