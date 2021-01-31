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
#include <machine.h>

#include "../users.h"
#include "../process.h"
#include "../context.h"

/* Print out all defined processes */
int ps(int argc, char *argv[]) {
    printf("%s %13s %13s %15s %15s %15s\n\r", "PID", "User", "CMD", "State", "Stack", "PC");
    for(int pid=0;pid<MAX_PROCESSES;pid++) {
        if (processes[pid].name != NULL) {
            printf("%d %15s %15s %15s %#15x %#15x\n\r",
                pid,
                uid_lookup(processes[pid].owner)->name,
                processes[pid].name,
                process_state(get_state(pid)),
                processes[pid].context->usp,
                processes[pid].context->pc
            );
        }
    }
    
    return 0;
}