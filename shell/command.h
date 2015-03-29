#ifndef __shell__command__ 
#define __shell__command__
#include "ArgumentList.h"
#include <unistd.h>

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
 Create a new command with default I/O (stdin and stdout)
 with a given command and argument list
 */
Command newCommand(char* executable, StringList* args);

void freeCommand(Command);

/**
 Fork/exec a single command and return the pid of the created command
 */
pid_t executeCommand(Command);

/**
 Print debug information about a command
 */
void printCommand(Command);

#endif /* defined(__shell__command__) */
