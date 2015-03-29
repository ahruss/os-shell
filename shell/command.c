#include "command.h"
#include "shell.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
    c->input = STDIN_FILENO;
    c->output = STDOUT_FILENO;
    return c;
}

void freeCommand(Command c) {
    free(c->executable);
    freeList(c->args);
    free(c);
}

/**
 tries to create a new process for the command to execute
 returns the PID of the created process. If we fail to 
 create the process for some reason, returns < 0
 */
pid_t executeCommand(Command c) {
    int argsCount = listLength(c->args);

    // need 1 for program + # args + NULL terminator
    char** args = malloc(sizeof(char*) * (1 + argsCount + 1));
    args[argsCount+1] = 0;

    args[0] = c->executable;
    for (int k = 0; k < argsCount; ++k) {
        args[k + 1] = findElement(c->args, k);
    }

    // figure out which program to run
    char* program = which(c->executable);

    // if there wasn't a program to execute or we can't fork a new
    // process for some reason, indicate failure
    pid_t forked;
    if (program == NULL) {
        lastShellError = "Unknown program";
        return -1;
    } else if ((forked = fork()) == -1) {
        lastShellError = "Failed to fork process.";
        return -1;
    } else if (forked == 0) {
        // Child process; the command we executed.

        // Set the file descriptors for IO
        dup2(c->input, STDIN_FILENO);
        dup2(c->output, STDOUT_FILENO);

        // start the comand
        execv(program, args);
        return -1;
    } else {
        free(args);
        free(program);
        // I'm the parent process, return the PID of the process we spawned
        return forked;
    }
}