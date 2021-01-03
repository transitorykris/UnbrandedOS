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
#include <debug_stub.h>
#include <easy68k/easy68k.h>
#include <machine.h>

#include "helpers.h"
#include "context.h"
#include "system.h"
#include "mfp.h"
#include "trap14.h"

#include "shell.h"

struct context_t *current_process;  // Currently executing task

void create_process(struct context_t *context, uint32_t pc, uint32_t sp);

noreturn void kmain() {
  // Set up the handy crash dump printer
  debug_stub();

  // The rosco-m68k needs a bit of time (why?)
  for (int i=0;i<50000;i++);

  e68ClearScr();
  e68Println("Kernel starting");

  // This context gets trashed after the first context switch
  struct context_t throw_away = {
    .usp = 0x6000,
    //.pc is populated after the first context switch
    .state = SLEEPING,
  };
  current_process = &throw_away;
  current_process->next = current_process;  // only process in the list

  mcPrintln("Setting up shell");
  struct context_t pid1;
  create_process(&pid1, (uint32_t)shell, 0x10000);

  // Overwrite trap14 vector -- small hack so we don't have to burn ROMs
  mcPrintln("Overriding ROM IO");
  SET_VECTOR(TRAP_14_HANDLER, TRAP_14_VECTOR);

  // Start the timer for firing the scheduler
  mcPrintln("Starting scheduler");
  context_init();
  SET_VECTOR(context_swap, MFP_TIMER_C);

  // Ready to go
  mcPrintln("Entering user mode");
  set_usp(current_process->usp);  // keep the supervisor stack clean
  disable_supervisor();

  // We never return, but we will also stop execution here after the
  // first context switch
  for (;;);
}

void create_process(struct context_t *context, uint32_t pc, uint32_t sp) {
  context->pc = pc;
  context->usp = sp;
  context->sr = 0x00;

  // Initialize our registers to zer
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
}