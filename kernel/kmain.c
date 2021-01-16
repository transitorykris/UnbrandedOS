/*
Copyright 2020 Kris Foster

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

#include <debug_stub.h>

#include "helpers.h"
#include "context.h"
#include "system.h"
#include "mfp.h"
#include "trap14.h"
#include "process.h"
#include "fs.h"
#include "malloc.h"
#include "syscall.h"

#include "shell.h"

void init_processes() {
    for (int i=0;i<MAX_PROCESSES;i++) {
        processes[i].name = NULL;
        processes[i].context = NULL;
    }
}

void init_interrupts() {
    // Overwrite trap14 vector -- small hack so we don't have to burn ROMs
    SET_VECTOR(TRAP_14_HANDLER, TRAP_14_VECTOR);
    SET_UINT32_VECTOR(syscall_handler, TRAP_0_VECTOR);
}

void init_scheduler() {
    context_init();
    SET_VECTOR(context_swap, MFP_TIMER_C);
}

noreturn void kmain() {
    init_processes();

    debug_stub();   // Set up the handy crash dump printer

    delay(40000);   // The rosco-m68k needs a bit of time (why?)

    printf("\033[2JKernel starting\n\r");

    printf("Initializing pages %#x to %#x\n\r", (uint32_t)HEAP_START,
        (uint32_t)HEAP_END);
    init_heap();

    printf("Initializing file system\n\r");
    init_filesystem();

    // This context gets trashed after the first context switch
    struct context_t idle_proc = {
        .usp = 0x6000,
        //.pc is populated after the first context switch
        .state = SLEEPING,
    };
    current_process = &idle_proc;
    current_process->next = current_process;  // only process in the list
    processes[0].name = "idle";
    processes[0].context = &idle_proc;

    printf("Setting up shell as PID1\n\r");
    struct context_t pid1;
    create_process(&pid1, "shell", (uint32_t)shell, 0x10000);
    processes[1].context = &pid1;

    printf("Initializing interrupts\n\r");
    init_interrupts();

    // Start the timer for firing the scheduler
    printf("Starting scheduler\n\r");
    init_scheduler();

    // Ready to go
    set_usp(current_process->usp);  // keep the supervisor stack clean
    disable_supervisor();

    // We never return, but we will also stop execution here after the
    // first context switch
    sleep();
}
