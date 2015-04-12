#include "command.h"
#include "job.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

void printJob(Job j) {
    printf("job\n");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < j->commandCount; ++i) {
        printCommand(j->commands[i]);
    }
    printf("--------------------------------------------------------------------------------\n");
}

Job newJob() {
    Job job = malloc(sizeof(job_t));
    job->commandCount = 0;
    job->commandsCapacity = 0;
    job->commands = NULL;
    return job;
}

void freeJob(Job j) {
    if (j->commands != NULL) {
        for (int i = 0; i < j->commandCount; ++i) {
            freeCommand(j->commands[i]);
        }
        free(j->commands);
    }
    free(j);
}

int waitFor(pid_t p) {
    int status;
    waitpid(p, &status, 0);
    return WEXITSTATUS(status);
}

/**
 Adds a command to the job's command array, increasing its size if necessary
 */
void pushCommand(Command c, Job j) {
    if (j->commands == 0) {
        // if we haven't created the table yet, create it
        j->commands = malloc(sizeof(Command));
        j->commandCount = 0;
        j->commandsCapacity = 1;
    } else if (j->commandsCapacity == j->commandCount) {
        // if there's not enough room in our table, increase its size
        Command* oldCommands = j->commands;
        j->commandsCapacity *= 2;
        j->commands = malloc(sizeof(Command) * j->commandsCapacity);
        for (int i = 0; i < j->commandCount; ++i) {
            j->commands[i] = oldCommands[i];
        }
        free(oldCommands);
    }
    j->commands[j->commandCount] = c;
    ++j->commandCount;
}

void pipeCommandTo(Command c, Job j) {
    // get the command this one is piped to
    Command target = j->commands[j->commandCount - 1];
    // add this command to table
    pushCommand(c, j);
    // set the input and output for these commands in the table
    int pipeDescriptors[2];
    if (pipe(pipeDescriptors) == -1) {
        lastShellError = "Couldn't acquire file descriptors for pipe";
        exit(EXIT_FAILURE);
    }
    c->output = pipeDescriptors[PIPE_WRITE];
    c->inputToClose = pipeDescriptors[PIPE_READ];
    target->input = pipeDescriptors[PIPE_READ];
    target->outputToClose = pipeDescriptors[PIPE_WRITE];
}

int executeJob(Job j, redirect_t* in, redirect_t* out,  redirect_t* err, bool inBackground) {

    for (int i = 0; i < j->commandCount; ++i) {
        if (j->commands[i]->executable == NULL) {
            lastShellError = "Unknown environment variable.";
            freeJob(j);
            return -1;
        }
    }

    if (!setStdin(j, in) || !setStdout(j, out) || !setStderr(j, err)) {
        return 1;
    }
    free(in);
    free(out);
    free(err);

    int count = j->commandCount;

    pid_t pids[count];
    for (int i = 0; i < count; ++i) {
        Command c = j->commands[i];
        pids[i] = executeCommand(c, j);

        // failed to start executing the commands
        if (pids[i] < 0) {
            freeJob(j);
            return -1;
        }
    }

    freeJob(j);

    // if the job isn't supposed to run in the background, wait for the spawned processes to exit
    if (!inBackground) {
        awaitedProcessIds = pids;
        awaitedCount = count;
        for (int i = 0; i < count; ++i) {
            int code = waitFor(pids[i]);
            if (code != 0) {
                return code;
            }
        }
        awaitedProcessIds = NULL;
        awaitedCount = 0;
    }
    // if we make it down here all the exit codes were 0 (i.e., no error)
    return 0;
}

// MARK: - Utilities for Handling Redirects

Redirect makeRedirect(char* file, RedirectType mode)  {
    Redirect redirect = malloc(sizeof(redirect_t));
    redirect->file = file;
    redirect->mode = mode;
    return redirect;
}

bool setStdin(Job j, Redirect r) {
    if (j->commandCount == 0) return false;

    Command firstCommand = j->commands[0];
    switch (r->mode) {
        case RedirectTypeNone: return true;
        case RedirectTypeRead:
            {
                firstCommand->inputFile = fopen(r->file, "r");
                if (firstCommand->inputFile != NULL) firstCommand->input = fileno(firstCommand->inputFile);
                return firstCommand->inputFile != NULL;
            }
        default:
            {
                printf("Something weird happened: tried to set stdin with a type other than none or read");
                return false;
            }
    }
}

bool setStdout(Job j, Redirect r) {
    if (j->commandCount == 0) return false;
    Command lastCommand = j->commands[0];
    char* mode;
    switch (r->mode) {
        case RedirectTypeWrite: mode = "w"; break;
        case RedirectTypeAppend: mode = "a"; break;
        default: mode = NULL; return true;
    }
    lastCommand->outputFile = fopen(r->file, mode);
    if (lastCommand->outputFile != NULL) lastCommand->output = fileno(lastCommand->outputFile);
    return lastCommand->outputFile != NULL;
}

bool setStderr(Job j, Redirect r) {
    if (j->commandCount == 0) return false;
    Command lastCommand = j->commands[0];
    switch (r->mode) {
        case RedirectTypeWrite:
        {
            lastCommand->errorFile = fopen(r->file, "w");
            if (lastCommand->errorFile != NULL) lastCommand->error = fileno(lastCommand->errorFile);
            return lastCommand->errorFile != NULL;
        }
        case RedirectTypeEquals:
        {
            /* don't set the file because it only needs to be stored one time; do set the error file descriptor */
            lastCommand->error = dup(lastCommand->output);
        }
        default: return true;
    }
}
