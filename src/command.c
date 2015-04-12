#include "command.h"
#include "shell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void printCommand(Command c) {
    printf("printing command %p\n", c);
    printf("\texecutable: %s\n", c->executable);
    printf("\targs: %p\n", c->args);
    printf("\tinput: %d\n", c->input);
    printf("\toutput: %d\n", c->output);
}

Command newCommand(char* executable, StringList* args) {
    Command c = malloc(sizeof(command_t));
    c->executable = executable;
    c->args = args;

    c->inputFile = NULL;
    c->outputFile = NULL;
    c->errorFile = NULL;

    c->input = STDIN_FILENO;
    c->output = STDOUT_FILENO;
    c->error = STDERR_FILENO;

    c->inputToClose = -1;
    c->outputToClose = -1;

    return c;
}

void mclose(int n) {
    close(n);
}

void freeCommand(Command c) {
    if (c->inputToClose != -1) {
        mclose(c->inputToClose);
    }
    if (c->outputToClose != -1) {
        mclose(c->outputToClose);
    }
    if (c->input != STDIN_FILENO) {
        mclose(c->input);
    }
    if (c->output != STDOUT_FILENO) {
        mclose(c->output);
    }
    if (c->error != STDERR_FILENO) {
        mclose(c->error);
    }
    if (c->executable != NULL) {
        free(c->executable);
    }
    freeList(c->args);
    free(c);
}

pid_t __executeBuiltin(Command c) {
    int commandIndex = isBuiltin(c->executable);
    assert(commandIndex >= 0);
    // just execute in the current process; builtins need access to our address space
    executeBuiltin(c, commandIndex);
    return 0;
}

pid_t __executeNonBuiltin(Command c, char** args, Job j) {
    // figure out which program to run
    char* program = which(c->executable);

    // if there wasn't a program to execute, don't even try to fork a new process
    if (program == NULL) {
        lastShellError = "Unknown program";
        return -1;
    }

    pid_t forked = fork();
    if (forked == -1) {
        // If we can't fork the process, there's an error.
        lastShellError = "Failed to fork process.";
        return -1;
    } else if (forked == 0) {
        // Child process; the command we executed.

        if (c->inputToClose != -1) mclose(c->inputToClose);
        if (c->outputToClose != -1) mclose(c->outputToClose);

        // Set the file descriptors for IO
        if (c->input != STDIN_FILENO) dup2(c->input, STDIN_FILENO);
        if (c->output != STDOUT_FILENO) dup2(c->output, STDOUT_FILENO);
        if (c->error != STDERR_FILENO) dup2(c->error, STDERR_FILENO);

        for (int i = 0; i < j->commandCount; ++i) {
            if (j->commands[i] != c) {
                freeCommand(j->commands[i]);
            }
        }

        // start the comand
        execv(program, args);
        // if execv failed return failure
        return -1;
    } else {
        if (c->outputFile != NULL) { fclose(c->outputFile); }
        if (c->inputFile != NULL) { fclose(c->inputFile); }
        if (c->errorFile != NULL) { fclose(c->errorFile); }
        free(program);
        // I'm the parent process, return the PID of the process we spawned
        return forked;
    }
}

bool isAlias(char* executable) {
    AliasList* l = aliasList;
    for(int i = 0; i < aliasListLength(aliasList); i++) {
        if(strcmp(executable, l->alias) == 0) {
            return true;
        }
        l = l->next;
    }
    return false;
}

void getExecutable(Command c) {
    if(!isAlias(c->executable)) {
        return;
    }
    AliasList* l = aliasList;
    for(int i = 0; i < aliasListLength(aliasList); i++) {
        if(strcmp(c->executable, l->alias) == 0){
            StringList *newList = listCopy(l->argsList);
            if(newList !=NULL) {
                tailOf(newList)->next = c->args;
                c->args = newList;
            }
            free(c->executable);
            c->executable = strdup(l->value);
        }
        l = l->next;
    }
    getExecutable(c);
}


/**
 tries to create a new process for the command to execute
 returns the PID of the created process. If we fail to 
 create the process for some reason, returns < 0
 */
pid_t executeCommand(Command c, Job j) {

    // if any of the args are NULL, there was probably a problem with
    // variable expansion
    if (listContains(c->args, NULL)) {
        lastShellError = "Unknown variable";
        return -1;
    }
    c->args = expandTildes(c->args);
    c->args = expandWildcards(c->args);

    getExecutable(c);
    int argsCount = listLength(c->args);

    // need 1 for program + # args + NULL terminator
    char** args = malloc(sizeof(char*) * (1 + argsCount + 1));
    args[argsCount+1] = 0;

    args[0] = c->executable;
    for (int k = 0; k < argsCount; ++k) {
        args[k + 1] = findElement(c->args, k);
    }

    pid_t spawnedProcess = -1;
    if (isBuiltin(c->executable) != -1) {
        spawnedProcess = __executeBuiltin(c);
    }
    else {
        spawnedProcess = __executeNonBuiltin(c, args, j);
    }
    free(args);
    return spawnedProcess;
}

