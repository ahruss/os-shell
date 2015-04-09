#include <stdio.h>
#include <string.h>
#include "builtins.h"

#define NUMBUILTINS 7

extern char **environ;

int set_env(StringList *stringList);
int printenv(StringList *stringList);
int unset_env(StringList *stringList);
int cd(StringList *stringList);
int alias(StringList *stringList);
int unalias(StringList *stringList);
int bye(StringList *stringList);

char* builtin_commands[] = {
    "setenv",
    "printenv",
    "unsetenv",
    "cd",
    "alias",
    "unalias",
    "bye"
};

int (*builtin_functions[]) (StringList *) = {
    &set_env,
    &printenv,
    &unset_env,
    &cd,
    &alias,
    &unalias,
    &bye
};

int set_env(StringList *stringList) {
    if(listLength(stringList) != 2) {
        fprintf(stderr, "Error: There must be exactly two arguments for setenv");
        return -1;
    }
    if(setenv(findElement(stringList, 0), findElement(stringList, 1), 1) != 0) {
        fprintf(stderr, "Error: There is insufficient space in the environment. ");
    }
    return 1;
}

int printenv(StringList *stringList) {
    char *var = *environ;
    for (int i = 1; var; i++) {
        printf("%s\n", var);
        var = *(environ+i);
    }
    return 1;
}

int unset_env(StringList *stringList) {
    if(listLength(stringList) != 1) {
        fprintf(stderr, "Error: There must be exactly 1 argument for unset_env");
        return -1;
    }
    unsetenv(findElement(stringList, 0));
    return 1;
}

int cd(StringList *stringList) {
    printf("L");
    if(listLength(stringList)  > 1) {
        fprintf(stderr, "Error: There must be either 0 or 1 argument to cd");
        return -1;
    }
    if(listLength(stringList) == 0) {
        chdir(getenv("HOME"));
    } else {
        char* str = findElement(stringList, 0);
        if(*str == '~') {
            str++;
            if(*str == '\0') {
                chdir(getenv("HOME"));
            } else {
                char *s = str;
                int length = 0;
                //get length from char after ~ to end or /
                while(*s != '\0' && *s != '/'){
                    length++;
                    s++;
                }
                //length + null char
                char name[length + 1];
                for(int i = 0; i < length; i++) {
                    name[i] = *str;
                    str++;
                }
                name[length] = '\0';
                //get rest of the string, use for later after directory switch.
                int stringLen = 0;
                while(*s != '\0') {
                    s++;
                    stringLen++;
                }
                char dir[stringLen + 1];
                for(int i = 0; i < stringLen; i++) {
                    dir[i] = *str;
                    str++;
                }
                dir[stringLen] = '\0';
                /* 
                 look up the substring in /etc/passwd using getpwnam() and extract the user's 
                 home directory from the returned struct.
                 */
                struct passwd *pw;
                pw = getpwnam(name);
                if(pw == NULL) {
                    fprintf(stderr, "Error: invalid User");
                    return -1;
                }
                char *homeDir = (pw->pw_dir);
                char* combined = malloc(sizeof(char) * (strlen(homeDir) + strlen(dir) + 1));
                strcpy(combined, homeDir);
                strcat(combined, dir);
                combined[strlen(homeDir) + strlen(dir)] = '\0';
                if(chdir(combined) != 0) {
                    free(combined);
                    fprintf(stderr, "Error: invalid directory");
                    return -1;
                } else {
                    free(combined);
                }
            }
        } else {
            if(chdir(str) != 0) {
                fprintf(stderr, "Error: invalid directory or not enough perissions");
                return -1;
            }
        }
    }
    return 1;
}

int alias(StringList *stringList) {
    //list all aliases in AliasTable
    if(listLength(stringList) == 0) {
        printAliasList();
    } else {
        printf("LENGTH: %d", listLength(stringList));
        if (!(listLength(stringList) == 2)) {
            fprintf(stderr, "Error: There must be either 0 or 2 arguments to alias");
            return -1;
        }
        StringList *argsList = parseAliasArgs(stringList);
        if(listLength(argsList) == 2) {
            aliasListPush(aliasList, findElement(argsList, 0), findElement(argsList, 1), 0);
        } else {
            aliasListPush(aliasList, findElement(argsList, 0), findElement(argsList, 1), nodeAtIndex(argsList, 2));
        }
    }
    return 1;
}

int unalias(StringList *stringList) {
    if(listLength(stringList) != 1) {
        fprintf(stderr, "Error: There must be 1 argument to unalias");
        return -1;
    }
    aliasList = aliasListRemove(aliasList, findElement(stringList, 0));
    return 1;
}

int bye(StringList *stringList) {
    if(listLength(stringList) != 0) {
        fprintf(stderr, "Error: There must be either 0 arguments to bye");
        return -1;
    }
    exitShell();
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

int executeBuiltin(char* command, StringList* args, int commandIndex) {
    // TODO: implement builtins
    printf("Executing builtin: %s, args %p", command, args);
    return (*builtin_functions[commandIndex])(args);
}
