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

#include "context.h"
#include "process.h"
#include "system.h"

#include "wait.h"

// Add's the current process to pid's wait list
int _add_wait_list(pid_t pid) {
    struct pcb_t *child = processes[pid].pcb;
    for(int i=0;i<MAX_WAIT_LIST;i++) {
        if (child->wait_list[i] == 0) {
            child->wait_list[i] = current_process->pid;
            return 0;
        }
    }
    return -1;
}

pid_t wait(int *stat_loc) {
    // XXX feels like there is a race condition lurking around here
    return waitpid(-1, stat_loc, 0);
}

pid_t waitpid(pid_t pid, int *stat_loc, int options) {
    if (pid == -1) {
        // We are going to wait on any child to exit
        for (int i=0;i<MAX_CHILDREN;i++) {
            if (current_process->children[i] != 0) {
                _add_wait_list(current_process->children[i]);
            }
        }
    } else {
        _add_wait_list(pid);
    }
    current_process->state = SLEEPING;  // child will wake us
    // XXX we really need a wait to initiate a context swap
    // this is a quick hack
    delay(10000);
    // TODO this needs to be the returning child's pid
    return 0;
}