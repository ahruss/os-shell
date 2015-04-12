#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <glob.h>
#include <termios.h>
#include "shell.h"
#include "all.h"

int lastReturnCode = 0;
int lastErrorCode = 0;
char* lastShellError = NULL;
int lineNumber = 0;

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
        char* dir = NULL;
        if (n->data[0] != '~' || (dir = getDirectoryFromUser(n->data)) == NULL) {
            dir = strdup(n->data);
        }
        // join the paths together
        unsigned long totalLength = strlen(command) + strlen(dir) + 1 + 1;
        char* combined = malloc(totalLength);
        strcpy(combined, dir);
        // this might add extra slashes, but that's fine according to UNIX standard
        strcat(combined, "/");
        strcat(combined, command);

        free(dir);
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
    // if there was an error, return 1; else return 0
    exit(lastShellError != 0);
}

char* getPrompt() {
    return strdup("$ ");
}

char* expandVariable(char* name) {
    char* value = getenv(name);
    return value;
}

StringList* globPath(char* pattern) {
    StringList* list = NULL;
    glob_t g;
    glob(pattern, GLOB_NOCHECK, NULL, &g);
    if (g.gl_pathc > 0) {
        list = newStringList(g.gl_pathv[0]);
        pattern = strdup(g.gl_pathv[0]);
    }
    for (int i = 1; i < g.gl_pathc; ++i) {
        list = listPush(list, g.gl_pathv[i]);
    }
    globfree(&g);
    return list;
}

StringList* expandWildcards(StringList* list) {
    StringList* first = list;
    StringList* previous = NULL;
    StringList* node = list;
    while (node != NULL) {
        printf("Node data: %s\n", node->data);
        StringList* expanded = globPath(node->data);
        if (previous == NULL) {
            first = expanded;
        } else {
            previous->next = expanded;
        }
        previous = tailOf(expanded);
        previous->next = node->next;
        node = node->next;
    }
    return first;
}

StringList* expandTildes(StringList* list) {
    StringList* node = list;
    while (node != NULL) {
        if (node->data[0] == '~') {
            // remove the tilde from th username
            char* username = node->data + 1;
            char* dir = getDirectoryFromUser(username);

            // if it's a real user, replace it with the expanded version
            if (dir != NULL) {
                char* old = node->data;
                node->data = dir;
                free(old);
            }
        }
        node = node->next;
    }
    return list;
}



bool isWhitespace(char c) {
    return c == ' ' || c == '\t';
}

static struct termios orig_termios;  /* TERMinal I/O Structure */

void tty_raw(void) {
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    old.c_cc[VINTR] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
}

void tty_reset(void) {
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    old.c_cc[VINTR] = 1;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
}

char* expand(char* lastWord) {
    if (lastWord[0] == '~') {
        // expand username
        return getDirectoryFromUser(lastWord + 1);
    } else {
        long copyLength = strlen(lastWord);
        char* copy = malloc(sizeof(char) * (copyLength + 2));
        strcpy(copy, lastWord);
        copy[copyLength] = '*';
        copy[copyLength+1] = '\0';
        // expand paths
        StringList* paths = globPath(copy);
        if (listLength(paths) == 1 && strcmp(copy, findElement(paths, 0)) != 0) {
            char* result = strdup(findElement(paths, 0));
            freeList(paths);
            return result;
        }
    }
    return NULL;
}

#define INITIAL_STRING_SIZE 255
char* getNextLine() {
    char* string = NULL;
    int size = 0;
    int length = 0;
    tty_raw();
    while (true) {
        char c = getchar();
        if (c == 27) {
            // escape, expand paths

            // find the beginning of the last word
            char* cursor = &string[length];
            while (cursor != string && !isWhitespace(cursor[-1])) {
                cursor--;
            }
            long wordSize = &string[length] - cursor;

            // copy the last word to a new buffer
            char* lastWord = malloc(sizeof(char) * (wordSize + 2));
            for (int i = 0; i < wordSize; ++i) {
                lastWord[i] = cursor[i];
            }
            lastWord[wordSize] = '\0';

            // get the expansion
            char* replacement = expand(lastWord);
            free(lastWord);

            // if we have an expansion, actually replace it
            if (replacement != NULL) {
                // create the new command
                *cursor = '\0';
                long newStringLength = strlen(string) + strlen(replacement) + 1;
                char* newString = malloc(sizeof(char) * newStringLength);
                strcpy(newString, string);
                strcat(newString, replacement);
                
                // replace the typed input with the expanded version
                printf("\n$ %s", newString);
                fflush(stdout);

                free(replacement);
                free(string);
                string = newString;
                length = (int)newStringLength - 1;
                size = length + 1;
            }
        } else if (c == 3) {
            printf("^C\n");
            // Ctrl-C
            if (string != NULL) {
                free(string);
            }
            tty_reset();
            return strdup("\n");
        } else if (c == 4) {
            // Ctrl-D (EOF)
            exitShell();
            break;
        } else if (c == '\b' || c == 127) {
            // backspace
            if (length != 0) {
                length--;
                printf("\b \b");
            }
        } else {
            // normal characters
            if (string == NULL) {
                string = malloc(sizeof(char) * INITIAL_STRING_SIZE);
                size = INITIAL_STRING_SIZE;
            } else if (size == length) {
                char* newString = malloc(sizeof(char) * (size * 2));
                size *= 2;
                strcpy(newString, string);
                free(string);
                string = newString;
            }
            string[length] = c;
            length++;
            printf("%c", c);
            if (c == 13 || c == '\n') {
                break;
            }
        }
    }
    if (size == length) {
        char* newString = malloc(sizeof(char) * (size * 2));
        size *= 2;
        strcpy(newString, string);
        free(string);
        string = newString;
    }

    if (string != NULL) {
        string[length] = '\0';
    }
    tty_reset();
    return string;
}

void initShell() {
    tcgetattr(STDIN_FILENO, &orig_termios);
}

