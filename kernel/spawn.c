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

#include <stdio.h>
#include <string.h>

#include "malloc.h"

#include "fs.h"

#include "spawn.h"

/* posix_spawn
pid - returns the child pid
path - path to executable
file_actions - (to be implemented)
attrp - (to be implemented)
*argv - argv[0] file name for process, array terminated by null pointer
*envp - (to be implemented) environment for new process, array terminated
        by null pointer

Return 0 on success, error code otherwise
*/
int posix_spawn(pid_t *restrict pid, const char *restrict path,
       const posix_spawn_file_actions_t *file_actions,
       const posix_spawnattr_t *restrict attrp,
       char *const argv[restrict], char *const envp[restrict]) {
    
    void *entry;     // Entry point to the executable

    // Count the number of arguments
    int argc = 0;
    for (int i=0;i<MAX_ARGS;i++) {
        if (argv[i] == NULL) {
            argc = i;
            break;
        }
    }

    // Find the executable
    for (int i=0;i<MAX_FILES;i++) {
        if (!strcmp(fs.root->files[i].name, path)) {
            entry = fs.root->files[i].inode.start;
            break;
        }
    }
    if (!entry) {
        return FILE_NOT_FOUND;
    }

    // Get a fresh stack for this process
    struct context_t *context = \
        (struct context_t*)malloc(sizeof(struct context_t));
    context->pc = (uint32_t)entry;
    // Stacks grow downward! Start at the highest value in the stack
    context->stack_base = \
        (uint32_t *)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
    context->usp = context->stack_base;
    context->sr = 0x0000;

    // Initialize our registers to zero
    for (int i=0;i<8;i++) {
        context->d[i] = 0x0000;
    }

    for (int i=0;i<7;i++) {
        context->a[i] = 0x0000;
    }

    // Set to running
    // XXX this should probably be embyonic until we're done?
    context->state = RUNNING;

    // Not pretty, but if this is the first process...
    if (processes[0].name == NULL) {
        current_process = context;
        current_process->next = current_process;
    }
    // Insert into the linked list
    context->next = current_process->next;
    current_process->next = context;

    pid_t _pid;
    for (_pid=0;_pid<MAX_PROCESSES;_pid++) {
        if (processes[_pid].name == NULL) {
            processes[_pid].name = argv[0];
            processes[_pid].context = context;
            processes[_pid].owner = 0;  // XXX fix me
            *pid = _pid;    // Return pid to the caller
            break;
        }
        // Note: we rely on learning the pid # in this loop!
        // so don't monkey with pid after this!
    }
    // XXX check this, does the for loop actually count this high?
    if (_pid == MAX_PROCESSES) {
        return ERR_TOO_MANY_PROCS;
    }

    return 0;
}