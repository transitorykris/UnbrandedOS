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

#include "fs/fs.h"
#include "stack.h"

#include "spawn.h"

// Add a child PID to a parent's children list
int _add_child(pid_t child) {
    // TODO a couple calls to wait() can create duplicates, check
    for(int i=0;i<MAX_CHILDREN;i++) {
        if (current_process->children[i] == 0) {
            current_process->children[i] = child;
            return 0;
        }
    }
    return -1;
}

// Remove a child's PID from parent's children list
int _remove_child(pid_t child) {
    struct context_t *parent;
    parent = processes[current_process->parent].context;
    for(int i=0;i<MAX_CHILDREN;i++) {
        if(parent->children[i] == current_process->pid) {
            parent->children[i] = 0;
            return 0;
        }
    }
    return -1;
}

// TODO will need some way to remove children that are zombies

// Spawned processes return here
void _exit_spawn(void) {
    // Wake up all the processes waiting on this pid
    struct context_t *parent;
    for (int i=0;i<MAX_WAIT_LIST;i++) {
        if (current_process->wait_list[i] != 0) {
            parent = processes[current_process->wait_list[i]].context;
            if (parent->state == SLEEPING) {
                parent->state = RUNNING;    // Start'r up
            }
        }
    }

    // Remove ourselves from the parent
    if (_remove_child(current_process->pid)) {
        printf("_exit_spawn: failed to remove child\n\r");
    }

    // Die
    current_process->state = ZOMBIE;
    for(;;);    // Do nothing until we stop scheduling this task
}

// Return the number of elements in argv
int arg_count(char *const argv[restrict]) {
    int argc = 0;
    for (int i=0;i<MAX_ARGS;i++) {
        if (argv[i] == NULL) {
            argc = i;
            break;
        }
    }
    return argc;
}

// Create a blank context
struct context_t *new_context(uint32_t *entry) {
    struct context_t *context = \
        (struct context_t*)malloc(sizeof(struct context_t));

    // Initialize the new process's registers to zero
    for (int i=0;i<8;i++) {
        context->d[i] = 0x0000;
        context->a[i] = 0x0000;
    }

    // Embryonic until we're ready for the scheduler to run this
    context->state = EMBRYO;

    // Stacks grow downward! Start at the highest value in the stack
    context->stack_base = \
        (uint32_t *)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
    context->usp = context->stack_base;

    // Clear SR
    context->sr = 0x0000;

    // The scheduler will start the process at this entry point
    context->pc = (uint32_t)entry;

    // Clear out the children and wait_list
    // Should really be using calloc
    for (int i=0;i<MAX_CHILDREN;i++) {
        context->children[i] = 0;
    }
    for (int i=0;i<MAX_WAIT_LIST;i++) {
        context->wait_list[i] = 0;
    }

    return context;
}

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
    int argc = arg_count(argv);

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

    struct context_t *context = new_context(entry);

    // Push values for the call to the process's main()
    push(context->usp, (uint32_t)argv);
    push(context->usp, argc);
    push(context->usp, (uint32_t)_exit_spawn);

    // Insert into the linked list
    context->next = current_process->next;
    current_process->next = context;

    pid_t _pid;
    for (_pid=0;_pid<MAX_PROCESSES;_pid++) {
        if (processes[_pid].name == NULL) {
            processes[_pid].name = argv[0];
            processes[_pid].context = context;
            processes[_pid].owner = 0;  // XXX fix me
            break;
        }
        // Note: we rely on learning the pid # in this loop!
        // so don't monkey with pid after this!
    }
    // XXX check this, does the for loop actually count this high?
    if (_pid == MAX_PROCESSES) {
        return ERR_TOO_MANY_PROCS;
    }

    // Add this child to parent's list
    if(_add_child(_pid) == -1) {
        context->state = ZOMBIE;    // Kill it!
        return TOO_MANY_CHILDREN;
    }

    // Save the pid in the context to make other lookups easier
    context->pid = _pid;

    // Save the parent's PID in the child
    context->parent = current_process->pid;

    // We're runnable now!
    context->state = RUNNING;

    // Return pid to the caller, no errors
    *pid = _pid;
    return 0;
}