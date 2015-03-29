#include "command.h"
#include "job.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    target->input = pipeDescriptors[PIPE_READ];
}

int waitFor(pid_t p) {
    int status;
    waitpid(p, &status, 0);
    return WEXITSTATUS(status);
}

int executeJob(Job j) {
    pid_t pids[j->commandCount];
    for (int i = 0; i < j->commandCount; ++i) {
        Command c = j->commands[i];
        pids[i] = executeCommand(c);

        // failed to start executing the commands
        if (pids[i] <= 0) {
            freeJob(j);
            return -1;
        }
    }

    freeJob(j);
    for (int i = 0; i < j->commandCount; ++i) {
        int code = waitFor(pids[i]);
        if (code != 0) {
            return code;
        }
    }
    // if we make it down here all the exit codes were 0 (i.e., no error)
    return 0;
}

