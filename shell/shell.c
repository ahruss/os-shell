#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "shell.h"
#include "all.h"

int lastReturnCode = 0;
int lastErrorCode = 0;
char* lastShellError = NULL;


/**
 Get the list of directories in the PATH
 */
StringList* getPathList() {
    char *tofree, *path, *token;
    StringList* list = NULL;
    tofree = path = strdup(getenv("PATH"));
    assert(path != NULL);
    while ((token = strsep(&path, ":")) != NULL) {
        if (list == NULL) {
            list = newStringList(token);
        } else {
            listPush(list, token);
        }
    }
    free(tofree);
    return list;
}

char* which(char* command) {
    if (access(command, X_OK) != -1) {
        return command;
    }
    StringList* list = getPathList();
    for (StringList* n = list; n != NULL; n = n->next) {
        // join the paths together
        unsigned long totalLength = strlen(command) + strlen(n->data) + 1 + 1;
        char* combined = malloc(totalLength);
        strcpy(combined, n->data);
        // this might add extra slashes, but that's fine according to UNIX standard
        strcat(combined, "/");
        strcat(combined, command);

        // if we can execute a file at the path,
        // then return that path; otherwise keep looking
        if (access(combined, X_OK) != -1) {
            freeList(list);
            return strdup(combined);
        }
    }
    freeList(list);
    // if we didn't find a file just return null
    return NULL;
}


void exitShell() {
    printf("Got exit signal, exiting.\n");
    exit(lastShellError == 0);
}

char* getPrompt() {
    return strdup("$ ");
}
