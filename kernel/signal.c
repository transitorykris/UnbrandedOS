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

#include "signal.h"

#include "process.h"
#include "context.h"
#include "stack.h"
#include "spawn.h"

void default_abnormal_handler(int sig) {
    printf("in abnormal signal handler\n\r");
}

void default_ignore_handler(int sig) {
    printf("in ignore signal handler\n\r");
}

void default_stop_handler(int sig) {
    _exit_spawn();  // XXX doesn't feel _quite_ right
}

void default_continue_handler(int sig) {
    printf("in continue signal handler\n\r");
}

int kill(pid_t pid, int sig) {
    struct pcb_t *pcb = processes[pid].pcb;

    // Push the current PC onto the process's stack as a return value
    push(pcb->usp, pcb->pc);

    // Adjust the program counter to the start of the signal handler
    pcb->pc = (uint32_t)pcb->signal_handler[sig];
}

sig_t signal(int sig, sig_t func) {
    current_process->signal_handler[sig] = func;
}