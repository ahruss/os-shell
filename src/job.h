#ifndef __shell__job__
#define __shell__job__

#include "command.h"

// MARK: Type Definitions
/**
 A groupd of (usually) interrelated processes to be executed.
 */
typedef struct job_t {
    Command* commands;
    int commandCount;
    int commandsCapacity;
} job_t;

typedef job_t* Job;

typedef enum RedirectType {

    /**
     Append to the end of the file when writing it
     */
    RedirectTypeAppend,

    /**
     Rewrite the file as a new one
     */
    RedirectTypeWrite,

    /**
     Just read the file as stdin
     */
    RedirectTypeRead,

    /**
     Ties stdin back to stdout
     */
    RedirectTypeEquals,

    /** 
     Do nothing different to the job's output
     */
    RedirectTypeNone

} RedirectType;

typedef struct redirect_t {
    char* file;
    RedirectType mode;
} redirect_t;

typedef redirect_t* Redirect;

// MARK: Ctors and Dtors

Redirect makeRedirect(char* file, RedirectType mode);
void freeRedirect(Redirect);

/**
 Creates a new job and returns it
 */
Job newJob();
void freeJob(Job);

// MARK: Related Methods

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
int executeJob(Job, Redirect, Redirect, Redirect, bool);

bool setStdin(Job, Redirect);
bool setStdout(Job, Redirect);
bool setStderr(Job, Redirect);

#endif /* defined(__shell__job__) */
