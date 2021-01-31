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

#define SDCARD

#ifdef SDCARD
#include <sdfat.h>
#endif

#include "helpers.h"
#include "context.h"
#include "system.h"
#include "mfp.h"
#include "trap14.h"
#include "process.h"
#include "fs.h"
#include "malloc.h"
#include "syscall.h"
#include "users.h"

#include "shell.h"

// We're going to reuse the vectors set up by firmware for the
// bits in there I'm not ready to replace yet
uint32_t *vectors = 0x0;

void init_processes() {
    for (int i=0;i<MAX_PROCESSES;i++) {
        processes[i].name = NULL;
        processes[i].context = NULL;
    }
}

void install_vector_handler(int vector, uint32_t handler) {
    vectors[vector] = handler;
}

void init_interrupts() {
    // Syscalls will be trap #0
    install_vector_handler(VEC_TRAP_0_HANDLER, (uint32_t)syscall_handler);

    // Overwrite trap14 vector -- small hack so we don't have to burn ROMs
    // while hacking on how these routines work
    install_vector_handler(VEC_TRAP_14_HANDLER, (uint32_t)TRAP_14_HANDLER);
}

void init_scheduler() {
    context_init();
    install_vector_handler(VEC_TICK_HANDLER, (uint32_t)context_swap);
}

noreturn void idle() {
    for(;;);
}

noreturn void kmain() {
    debug_stub();   // Set up the handy crash dump printer

    delay(40000);   // The rosco-m68k needs a bit of time (why?)

    printf("\033[2JKernel starting\n\r");

#ifdef SDCARD
    printf("Initializing storage\n\r");
    if (!SD_check_support()) {
        printf("ROM does not have SD support\n\r");
        // TODO halt here
    }

    printf("Initializing FAT\n\r");
    if (!SD_FAT_initialize()) {
        printf("Failed to initialize FAT\n\r");
        // TODO halt here
    }
#endif

    printf("Initializing pages %#x to %#x\n\r", (uint32_t)HEAP_START,
        (uint32_t)HEAP_END);
    init_heap();

    printf("Initializing file system\n\r");
    init_filesystem();

    printf("Initializing process list\n\r");
    init_processes();

    printf("Initializing user database\n\r");
    init_users();
    uid_t root_uid = create_user("root");   // This will always be 0

    printf("Creating idle process\n\r");
    int pid0 = create_process("idle", (uint32_t)idle, root_uid);
    if (pid0 != 0) {
        printf("Expected to create pid0, got %d\n\r", pid0);
    }
    set_state(pid0, SLEEPING);

    printf("Setting up shell as PID1\n\r");
    int pid1 = create_process("shell", (uint32_t)shell, root_uid);
    if (pid1 != 1) {
        printf("Expected to create pid1, got %d\n\r", pid1);
    }

    printf("Initializing interrupts\n\r");
    init_interrupts();

    // Start the timer for firing the scheduler
    printf("Starting scheduler\n\r");
    init_scheduler();

    // Ready to go
    set_usp(processes[pid0].context->usp);  // leave the kernel stack
    disable_supervisor();                   // switch to user mode

    // We never return, but we will also stop execution here after the
    // first context switch
    idle();
}
