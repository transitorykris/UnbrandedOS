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

struct context_t {
    // Order matters here
    uint32_t d[8];          // Data registers D1-D7
    uint32_t a[7];          // Address registers A0-A6
    uint32_t usp;           // User stack pointer / A7
    uint32_t sr;            // Status register
    uint32_t pc;            // Program counter

    struct context_t* next; // Next process to run
    // Order shouldn't matter too much below this line
};

struct context_t *current_process;  // Currently executing task

noreturn void kmain() {
  // Set up the handy crash dump printer
  debug_stub();

  // The rosco-m68k needs a bit of time (why?)
  for (int i=0;i<50000;i++);

  e68ClearScr();
  e68Println("Kernel started");

  // This context gets trashed after the first context switch  
  struct context_t throw_away = {
    .usp = 0x6000,
  };
  current_process = &throw_away;

  struct context_t pid1 = {
    .usp = 0x10000,
    .pc = (uint32_t)shell,
  };
  pid1.next=&pid1;

  // Start the timer for firing the scheduler
  context_init();
  SET_VECTOR(context_swap, MFP_TIMER_C);

  // Overwrite trap14 vector -- small hack so we don't have to burn ROMs
  SET_VECTOR(TRAP_14_HANDLER, TRAP_14_VECTOR);

  // Ready to go
  set_usp(current_process->usp);  // keep the supervisor stack clean
  disable_supervisor();

  // We never return, but we also stop execution here after the
  // first context switch
  for (;;) {
    mcPrintln(".");
  }
}
