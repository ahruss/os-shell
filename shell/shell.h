//
//  shell.h
//  OSShell
//
//  Created by Alexander Russ on 3/27/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#ifndef OSShell_shell_h
#define OSShell_shell_h

#include <stdlib.h>
#include <sys/types.h>
#include "all.h"

extern int lastErrorCode;

/**
 Saves the last error to be printed when available
 */
extern char* lastShellError;

/**
 A single process to be executed.
 */
typedef struct Command {
    char* executable;
    StringList* args;
    /**
     The file descriptor number of the output for this command
     */
    int output;
    /**
     The file descriptor number of the input for this command
     */
    int input;
} Command;

/**
 A groupd of (usually) interrelated processes to be executed.
 */
typedef struct Job {
    Command** commands;
    int commandCount;
    int commandsCapacity;
} Job;

/**
 Creates a new job and returns it
 */
Job* newJob();

/**
 Create a new command with default I/O (stdin and stdout)
 with a given command and argument list
 */
Command* newCommand(char* executable, StringList* args);

/**
 Adds a command to the front of a job, which pipes its output to 
 the job that was previously in the front (i.e., the one at the 
 end of the job's list of commands)
 */
void pipeCommandTo(Command*, Job*);

/**
 Push a command unrelated to the others onto a job's list
 */
void pushCommand(Command*, Job*);

/**
 Starts all the processes in the Job's queue and waits until they've completed.
 */
int executeJob(Job*);

/**
 Print out the prompt
 */
void prompt();


/**
 Fork/exec a single command and return the pid of the created command
 */
pid_t executeCommand(Command*);
#endif
