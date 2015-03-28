#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "shell.h"
#include "all.h"

#define PIPE_READ 0
#define PIPE_WRITE 1

int lastErrorCode = 0;
char* lastShellError = NULL;

void printCommand(Command* c) {
    printf("printing command %p\n", c);
    printf("\texecutable: %s\n", c->executable);
    printf("\targs: %p\n", c->args);
    printf("\tinput: %d\n", c->input);
    printf("\toutput: %d\n", c->output);
}

void printJob(Job* j) {
    printf("job\n");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < j->commandCount; ++i) {
        printCommand(j->commands[i]);
    }
    printf("--------------------------------------------------------------------------------\n");
}

Job* newJob() {
    Job* job = malloc(sizeof(Job));
    job->commandCount = 0;
    job->commands = 0;
    job->commandsCapacity = 0;
    return job;
}

/**
 Adds a command to the job's command array, increasing its size if necessary
 */
void pushCommand(Command* c, Job* j) {
    if (j->commands == 0) {
        // if we haven't created the table yet, create it
        j->commands = malloc(sizeof(Command*));
        j->commandCount = 0;
        j->commandsCapacity = 1;
    } else if (j->commandsCapacity == j->commandCount) {
        // if there's not enough room in our table, increase its size
        Command** oldCommands = j->commands;
        j->commandsCapacity *= 2;
        j->commands = malloc(sizeof(Command*) * j->commandsCapacity);
        for (int i = 0; i < j->commandCount; ++i) {
            j->commands[i] = oldCommands[i];
        }
        free(oldCommands);
    }
    j->commands[j->commandCount] = c;
    ++j->commandCount;
}

Command* newCommand(char* executable, StringList* args) {
    Command* c = malloc(sizeof(Command));
    c->executable = executable;
    c->args = args;
    c->input = STDIN_FILENO;
    c->output = STDOUT_FILENO;
    return c;
}

void pipeCommandTo(Command* c, Job* j) {
    // get the command this one is piped to
    Command* target = j->commands[j->commandCount - 1];
    // add this command to table
    pushCommand(c, j);
    // set the input and output for these commands in the table
    int pipeDescriptors[2];
    if (pipe(pipeDescriptors) == -1) {
        lastShellError = "Couldn't acquire file descriptors for pipe";
        exit(EXIT_FAILURE);
    }
//    printf("got pipe: %d %d", pipeDescriptors[0], pipeDescriptors[1]);
    c->output = pipeDescriptors[PIPE_WRITE];
    target->input = pipeDescriptors[PIPE_READ];
}

int waitFor(pid_t p) {
    int status;
    waitpid(p, &status, 0);
    return WEXITSTATUS(status);
}

int executeJob(Job* j) {
    pid_t pids[j->commandCount];
    for (int i = 0; i < j->commandCount; ++i) {
        Command* c = j->commands[i];
        pids[i] = executeCommand(c);

        // failed to start executing the commands
        if (pids[i] <= 0) {
            return -1;
        }
    }
    for (int i = 0; i < j->commandCount; ++i) {
        int code = waitFor(pids[i]);
        if (code != 0) return code;
    }
    // if we make it down here all the exit codes were 0 (i.e., no error)
    return 0;
}

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

/**
 Find the full path of an executable program given just the name 
 from somewhere in the path
 */
char* which(char* command) {
    if (access(command, X_OK) != -1) {
        return command;
    }
    StringList* list = getPathList();
    for (StringList* n = list; n != NULL; n = n->next) {

        // FIXME assumes the paths end in slash; if they don't will fail
        // join the paths together
        unsigned long totalLength = strlen(command) + strlen(n->data) + 1 + 1;
        char* combined = malloc(totalLength);
        strcpy(combined, n->data);
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

/**
 tries to create a new process for the command to execute
 returns the PID of the created process. If we fail to 
 create the process for some reason, returns < 0
 */
pid_t executeCommand(Command* c) {
    int argsCount = listLength(c->args);
    // need 1 for program + # args + NULL terminator
    char** args = malloc(sizeof(char*) * (1 + argsCount + 1));
    args[argsCount+1] = 0;

    args[0] = c->executable;
    for (int k = 0; k < argsCount; ++k) {
        args[k + 1] = findElement(c->args, k);
    }

    // figure out which program to run
    const char* program = which(c->executable);

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
        // I'm the parent process, return the PID of the process we spawned
        return forked;
    }
}

void prompt() {
    printf("$ ");
}
