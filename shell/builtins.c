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
    &set_env,
    &printenv,
    &unset_env,
    &cd,
    &alias,
    &unalias,
    &bye
};

int set_env(char **args) {
    return 1;
}

int printenv(char **args) {
    return 1;
}

int unset_env(char **args) {
    return 1;
}

int cd(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "expected an argument to cd");
        return -1;
    } else {
        if(chdir(args[1]) != 0) {
            fprintf(stderr, "Error: invalid directory");
            return -1;
        }
    }
    return 1;
}

int alias(char **args) {
    return 1;
}

int unalias(char **args) {
    return 1;
}

int bye(char **args) {
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