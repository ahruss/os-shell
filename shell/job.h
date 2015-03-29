#ifndef __shell__job__
#define __shell__job__

#include "command.h"

/**
 A groupd of (usually) interrelated processes to be executed.
 */
typedef struct job_t {
    Command* commands;
    int commandCount;
    int commandsCapacity;
} job_t;

typedef job_t* Job;

/**
 Creates a new job and returns it
 */
Job newJob();

void freeJob(Job);

/**
 Adds a command to the front of a job, which pipes its output to 
 the job that was previously in the front (i.e., the one at the 
 end of the job's list of commands)
 */
void pipeCommandTo(Command, Job);

/**
 Push a command unrelated to the others onto a job's list
 */
void pushCommand(Command, Job);

/**
 Starts all the processes in the Job's queue and waits until they've completed.
 */
int executeJob(Job);

#endif /* defined(__shell__job__) */
