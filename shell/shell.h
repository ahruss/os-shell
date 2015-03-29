#ifndef OSShell_shell_h
#define OSShell_shell_h

#include <stdlib.h>
#include <sys/types.h>
#include "all.h"

extern int lastReturnCode;

extern int lastErrorCode;
/**
 Saves the last error to be printed when available
 */
extern char* lastShellError;

/* need these to hide compiler warnings */
void yyerror(const char*);
int yylex();
int yyparse();

/**
 A single process to be executed.
 */
typedef struct command_t {
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
} command_t;

typedef command_t* Command;

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

/**
 Create a new command with default I/O (stdin and stdout)
 with a given command and argument list
 */
Command newCommand(char* executable, StringList* args);

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

/**
 Print out the prompt
 */
void prompt();

/**
 Clean up any required resources and exit the shell
 */
void exitShell();

/**
 Fork/exec a single command and return the pid of the created command
 */
pid_t executeCommand(Command);
#endif