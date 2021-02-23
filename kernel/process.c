/*
Copyright 2021 Kris Foster

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>

#include <machine.h>    // for some types

#include "sys/types.h"
#include "sys/wait.h"

#include "malloc.h"
#include "context.h"

#include "process.h"
#include "fs.h"

#define ERR_TOO_MANY_PROCS  -1

// !!! This needs to be behind a trap
// Interrupts need to be disabled or disaster will eventually strike

// create_process creates a new process given a name for it and
// the entry point in memory
// Returns the pid of the process or -1 if it failed
int create_process(char *name, uint32_t entry, uid_t owner) {
    struct context_t *context = (struct context_t*)malloc(sizeof(struct context_t));
    context->pc = entry;
    // Stacks grow downward! Start at the highest value in the stack
    context->usp = (uint32_t)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
    context->sr = 0x00;

    // Initialize our registers to zero
    for (int i=0;i<8;i++) {
        context->d[i] = 0x0000;
    }

    for (int i=0;i<7;i++) {
        context->a[i] = 0x0000;
    }

    // Set to running
    context->state = RUNNING;

    // Not pretty, but if this is the first process...
    if (processes[0].name == NULL) {
        current_process = context;
        current_process->next = current_process;
    }
    // Insert into the linked list
    context->next = current_process->next;
    current_process->next = context;

    // Add it to our process list
    int pid;
    for (pid=0;pid<MAX_PROCESSES;pid++) {
        if (processes[pid].name == NULL) {
            processes[pid].name = name;
            processes[pid].context = context;
            processes[pid].owner = owner;
            break;
        }
        // Note: we rely on learning the pid # in this loop!
        // so don't monkey with pid after this!
    }
    if (pid == MAX_PROCESSES) {
        // We ran out!
        return ERR_TOO_MANY_PROCS;
    }
    return pid;
}

char * process_state(uint8_t state) {
    switch(state) {
        case SLEEPING:
            return "sleeping";
        case RUNNING:
            return "running";
        default:
            return "unknown";
    }
}

state set_state(pid_t pid, state new_state) {
    state prev = processes[pid].context->state;
    processes[pid].context->state = new_state;
    return prev;
}

state get_state(pid_t pid) {
    return processes[pid].context->state;
}

// TO BE IMPLEMENTED
// https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html

pid_t fork(void) {
    // Copy the process
    // Set the new processes return register to the appropriate value
    // Set the program counter to the re-entry point
    // Return from this function with the appropriate value
    return -1;
}

pid_t wait(int *stat_loc) {
    return waitpid(-1, stat_loc, 0);
}

pid_t waitpid(pid_t pid, int *stat_loc, int options) {
    // Add this process to the other process's "wait list"
    // Also, create a waitlist for each process!
    // Also, how does that work?
    // return value when pid exits is stored in *stat_loc
    // return value of waitpid is the process id of the child
    // or -1 on error
    return -1;
}

// https://pubs.opengroup.org/onlinepubs/9699919799/

// Note: return value is likely not correct
int execvp(const char *file, char *const argv[]) {
    // XXX: We're temporarily going to create this ourselves
    char *temp_argv[1];
    int argc = sizeof temp_argv / sizeof (char *);

    for (int i=0;i<MAX_FILES;i++) {
        // XXX: file is just the name right now
        if (!strcmp(fs.root->files[i].name, file)) {
            temp_argv[0] = fs.root->files[i].name;
            // XXX this needs to be a create_process call
            // so that we get a stack properly set up
            fs.root->files[i].inode.start(argc, temp_argv);
            return 0;
        }
    }

    return 1;   // Generic error
}

void exit(int status) {
    // change the process state
    // never return
}