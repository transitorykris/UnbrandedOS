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
#include <machine.h>    // for some types

//#include "malloc.h"
#include "context.h"
#include "process.h"

void create_process(struct context_t *context, char *name, uint32_t entry) {
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

    // Insert into the linked list
    context->next = current_process->next;
    current_process->next = context;

    // Add it to our process list
    for (int i=0;i<MAX_PROCESSES;i++) {
        if (processes[i].name == NULL) {
            processes[i].name = name;
            processes[i].context = context;
            break;
        }
    }
    // We need to return an error if we ran out of process slots
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
