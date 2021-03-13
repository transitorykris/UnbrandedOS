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

#ifndef PROCESS_H
#define PROCESS_H

#include "sys/types.h"

#include "context.h"

#define MAX_PROCESSES       64
#define DEFAULT_STACK_SIZE  65536

// Process Control Block
typedef struct {
    char *name;                 // name used to create this process
    struct context_t *context;  // Complete state
    uid_t owner;                // Owner of this process
} pcb_t;

struct context_t *current_process;  // Currently executing task

// Pre-allocate all the processes we can run
pcb_t processes[MAX_PROCESSES];

int create_process(char *name, uint32_t entry, uid_t owner);
char * process_state(uint8_t state);
state set_state(pid_t pid, state new_state);
state get_state(pid_t pid);
void _trap_fork(void);
pid_t fork(void);
pid_t wait(int *stat_loc);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

#endif