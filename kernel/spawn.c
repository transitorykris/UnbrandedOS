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
#include "signal.h"

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
    struct pcb_t *parent;
    parent = processes[current_process->parent].pcb;
    for(int i=0;i<MAX_CHILDREN;i++) {
        if(parent->children[i] == current_process->pid) {
            parent->children[i] = 0;
            return 0;
        }
    }
    return -1;
}

// Spawned processes return here
void _exit_spawn(void) {
    // Wake up all the processes waiting on this pid
    struct pcb_t *parent;
    for (int i=0;i<MAX_WAIT_LIST;i++) {
        if (current_process->wait_list[i] != 0) {
            parent = processes[current_process->wait_list[i]].pcb;
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

// Sets the default signal handlers for every signal
void set_default_signal_handlers(struct pcb_t *pcb) {
    pcb->signal_handler[SIGNULL]    = default_ignore_handler;
    pcb->signal_handler[SIGABRT]    = default_abnormal_handler;
    pcb->signal_handler[SIGALRM]    = default_stop_handler;
    pcb->signal_handler[SIGBUS]     = default_abnormal_handler;
    pcb->signal_handler[SIGCHLD]    = default_ignore_handler;
    pcb->signal_handler[SIGCONT]    = default_continue_handler;
    pcb->signal_handler[SIGFP]      = default_abnormal_handler;
    pcb->signal_handler[SIGHUP]     = default_stop_handler;
    pcb->signal_handler[SIGILL]     = default_abnormal_handler;
    pcb->signal_handler[SIGINT]     = default_stop_handler;
    pcb->signal_handler[SIGKILL]    = default_stop_handler;
    pcb->signal_handler[SIGPIPE]    = default_stop_handler;
    pcb->signal_handler[SIGQUIT]    = default_abnormal_handler;
    pcb->signal_handler[SIGSEGV]    = default_abnormal_handler;
    pcb->signal_handler[SIGSTOP]    = default_continue_handler;
    pcb->signal_handler[SIGTERM]    = default_stop_handler;
    pcb->signal_handler[SIGTSTP]    = default_continue_handler;
    pcb->signal_handler[SIGTTIN]    = default_continue_handler;
    pcb->signal_handler[SIGTTOU]    = default_continue_handler;
    pcb->signal_handler[SIGUSR1]    = default_stop_handler;
    pcb->signal_handler[SIGUSR2]    = default_stop_handler;
    pcb->signal_handler[SIGPOLL]    = default_stop_handler;
    pcb->signal_handler[SIGPROF]    = default_stop_handler;
    pcb->signal_handler[SIGSYS]     = default_abnormal_handler;
    pcb->signal_handler[SIGTRAP]    = default_abnormal_handler;
    pcb->signal_handler[SIGURG]     = default_ignore_handler;
    pcb->signal_handler[SIGVTALRM]  = default_stop_handler;
    pcb->signal_handler[SIGXCPU]    = default_abnormal_handler;
    pcb->signal_handler[SIGXFSZ]    = default_abnormal_handler;
}

// Create a blank pcb
struct pcb_t *new_pcb(uint32_t *entry) {
    struct pcb_t *pcb = \
        (struct pcb_t*)malloc(sizeof(struct pcb_t));

    // Initialize the new process's registers to zero
    for (int i=0;i<8;i++) {
        pcb->d[i] = 0x0000;
        pcb->a[i] = 0x0000;
    }

    // Embryonic until we're ready for the scheduler to run this
    pcb->state = EMBRYO;

    // Stacks grow downward! Start at the highest value in the stack
    pcb->stack_base = \
        (uint32_t *)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
    pcb->usp = pcb->stack_base;

    // Clear SR
    pcb->sr = 0x0000;

    // The scheduler will start the process at this entry point
    pcb->pc = (uint32_t)entry;

    // Clear out the children and wait_list
    // Should really be using calloc
    for (int i=0;i<MAX_CHILDREN;i++) {
        pcb->children[i] = 0;
    }
    for (int i=0;i<MAX_WAIT_LIST;i++) {
        pcb->wait_list[i] = 0;
    }

    // Set up the default signal handlers
    set_default_signal_handlers(pcb);

    return pcb;
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
    
    void *entry = 0;     // Entry point to the executable

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

    struct pcb_t *pcb = new_pcb(entry);

    // Push values for the call to the process's main()
    push(pcb->usp, (uint32_t)argv);
    push(pcb->usp, argc);
    push(pcb->usp, (uint32_t)_exit_spawn);

    // Insert into the linked list
    pcb->next = current_process->next;
    current_process->next = pcb;

    pid_t _pid;
    for (_pid=0;_pid<MAX_PROCESSES;_pid++) {
        if (processes[_pid].name == NULL) {
            processes[_pid].name = argv[0];
            processes[_pid].pcb = pcb;
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
        pcb->state = ZOMBIE;    // Kill it!
        return TOO_MANY_CHILDREN;
    }

    // Save the pid in the pcb to make other lookups easier
    pcb->pid = _pid;

    // Save the parent's PID in the child
    pcb->parent = current_process->pid;

    // We're runnable now!
    pcb->state = RUNNING;

    // Return pid to the caller, no errors
    *pid = _pid;
    return 0;
}