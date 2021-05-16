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
#include <stdio.h>
#include <string.h>

#include <machine.h>    // for some types

#include "sys/types.h"
#include "sys/wait.h"
#include "sys/errors.h"

#include "malloc.h"
#include "context.h"

#include "process.h"
#include "fs/fs.h"
#include "syscall.h"

// !!! This needs to be behind a trap
// Interrupts need to be disabled or disaster will eventually strike

// create_process creates a new process given a name for it and
// the entry point in memory
// Returns the pid of the process or -1 if it failed
int create_process(char *name, uint32_t entry, uid_t owner) {
    struct pcb_t *pcb = (struct pcb_t*)malloc(sizeof(struct pcb_t));
    pcb->pc = entry;
    // Stacks grow downward! Start at the highest value in the stack
    pcb->stack_base = (uint32_t *)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
    pcb->usp = pcb->stack_base;
    pcb->sr = 0x0000;

    // Initialize our registers to zero
    for (int i=0;i<8;i++) {
        pcb->d[i] = 0x0000;
    }

    for (int i=0;i<7;i++) {
        pcb->a[i] = 0x0000;
    }

    // Set to running
    pcb->state = RUNNING;

    // Not pretty, but if this is the first process...
    if (processes[0].name == NULL) {
        current_process = pcb;
        current_process->next = current_process;
    }
    // Insert into the linked list
    pcb->next = current_process->next;
    current_process->next = pcb;

    // Add it to our process list
    int pid;
    for (pid=0;pid<MAX_PROCESSES;pid++) {
        if (processes[pid].name == NULL) {
            processes[pid].name = name;
            processes[pid].pcb = pcb;
            processes[pid].owner = owner;
            break;
        }
        // Note: we rely on learning the pid # in this loop!
        // so don't monkey with pid after this!
    }
    // XXX check this, does the for loop actually count this high?
    if (pid == MAX_PROCESSES) {
        // We ran out!
        return ERR_TOO_MANY_PROCS;
    }

    // Save the pid in the pcb to make other lookups easier
    pcb->pid = pid;

    return pid;
}

char * process_state(uint8_t state) {
    switch(state) {
        case SLEEPING:
            return "sleeping";
        case RUNNING:
            return "running";
        case EMBRYO:
            return "embryo";
        case ZOMBIE:
            return "zombie";
        default:
            return "unknown";
    }
}

state_t set_state(pid_t pid, state_t new_state) {
    state_t prev = processes[pid].pcb->state;
    processes[pid].pcb->state = new_state;
    return prev;
}

state_t get_state(pid_t pid) {
    return processes[pid].pcb->state;
}

// Fork the current process creating an almost identical copy
pid_t vfork(void) {
    syscall(VFORK);
}

// The heavy lifting of vfork is done here, called from vfork_handler
void _vfork(void) {
    // Allocate a new pcb for the copy
    struct pcb_t *new_pcb = (struct pcb_t*)malloc(sizeof(struct pcb_t));

    // Copy current_process to new_pcb
    for (int i=0; i<8; i++) {
        new_pcb->d[i] = current_process->d[i];  // a little abusive
    }
    for (int i=0; i<7; i++) {
        new_pcb->a[i] = current_process->a[i];  // a little abusive
    }
    new_pcb->sr = current_process->sr;
    new_pcb->pc = current_process->pc;

    // Set our return values
    new_pcb->d[0] = 0;              // Child's return value is 0
    current_process->d[0] = 0x123;      // Parent's return value is child's PID

    // Allocate a new stack for the copy
    // Stacks grow downward! Start at the highest value in the stack
    new_pcb->stack_base = (uint32_t *)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;

    // Copy the parent stack into the child stack -- stacks grow downard
    int stack_size = current_process->stack_base - current_process->usp;
    new_pcb->usp = new_pcb->stack_base - stack_size;
    memcpy(new_pcb->usp, current_process->usp, stack_size);

    for (int i=0;i<stack_size;i++) {
        printf("%#x\t%#x\t%#x\n\r", current_process->usp+i, new_pcb->usp[i], current_process->usp[i]);
    }

    new_pcb->state = SLEEPING;
    new_pcb->_errno = current_process->_errno;

    // Insert into the linked list
    new_pcb->next = current_process->next;
    current_process->next = new_pcb;
}

// https://pubs.opengroup.org/onlinepubs/9699919799/

// Note: return value is likely not correct
int execvp(const char *file, char *const argv[]) {
    // Count the number of arguments
    int argc = 0;
    for (int i=0;i<MAX_ARGS;i++) {
        if (argv[i] == NULL) {
            argc = i;
            break;
        }
    }

    // Find the file to execute
    for (int i=0;i<MAX_FILES;i++) {
        // XXX: file is just the name right now
        if (!strcmp(fs.root->files[i].name, file)) {
            fs.root->files[i].inode.start(argc, argv);
            return 0;
        }
    }
    // For now if we're here it means the file was not found
    errno = ENOENT;
    return -1;
}

void exit(int status) {
    // change the process state
    // never return
}
