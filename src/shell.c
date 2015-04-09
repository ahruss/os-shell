#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <glob.h>
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
        return strdup(command);
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

char* expandVariable(char* name) {
    char* value = getenv(name);
    return value;
}

StringList* expandWildcards(StringList* list) {
    StringList* node = list;
    while (node != NULL) {
        StringList* oldNext = node->next;
        glob_t g;
        glob(node->data, GLOB_NOCHECK, NULL, &g);
        if (g.gl_pathc > 0) {
            node->data = strdup(g.gl_pathv[0]);
        }
        for (int i = 1; i < g.gl_pathc; ++i) {
            node->next = newStringList(g.gl_pathv[i]);
            node = node->next;
        }
        node->next = oldNext;
        globfree(&g);
        node = oldNext;
    }
    return list;
}

StringList* getUserNames() {
    StringList *userNameList;
    struct passwd *p;
    p = getpwent();
    if(p != NULL) {
        userNameList = newStringList(p->pw_name);
    } else {
        return NULL;
    }
    while ((p = getpwent()) != NULL){
        listPush(userNameList, p->pw_name);
    }
    endpwent();
    return userNameList;
}

void initShell() {
    signal(SIGINT, interruptHandler);
}
