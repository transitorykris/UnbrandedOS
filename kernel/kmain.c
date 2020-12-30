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

struct context_t {
    // Order matters here
    uint32_t d[8];          // Data registers D1-D7
    uint32_t a[7];          // Address registers A1-A6
    uint32_t usp;           // User stack pointer / A7
    uint32_t sr;            // Status register (and really just CCR)
    uint32_t pc;            // Program counter

    struct context_t* next; // Next process to run
    // Order shouldn't matter too much below this line
};

struct context_t *current_process;

void tick_handler();

void user_routine_a();
void user_routine_b();

extern uint32_t test_process;

noreturn void kmain() {
  debug_stub();

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  e68ClearScr();
  e68Println("Kernel started");

  // This context gets trashed after the first context switch  
  struct context_t throw_away = {
    .d = {0xd1d1d1dd,0xd1d1d1dd,0x2d2d2ddd,0x3d3d3ddd,0x4d4d4ddd,0x5d5d5ddd,0x6d6d6ddd,0x7d7d7ddd},
    .a = {0xd1d1d1aa,0xd1d1d1aa,0x2d2d2daa,0x3d3d3daa,0x4d4d4daa,0x5d5d5daa,0x6d6d6daa},
    .usp = 0x6000,
  };
  current_process = &throw_away;

  struct context_t pid1_context = {
    .d = {0x010101dd,0x111111dd,0x212121dd,0x313131dd,0x414141dd,0x515151dd,0x616161dd,0x717171dd},
    .a = {0x010101aa,0x111111aa,0x212121aa,0x313131aa,0x414141aa,0x515151aa,0x616161aa},
    .usp = 0x10000,
    .pc = (uint32_t)user_routine_a,
  };
  current_process->next = &pid1_context;

  struct context_t pid2_context = {
    .d = {0x0f0f0fdd,0x1f1f1fdd,0x2f2f2fdd,0x3f3f3fdd,0x4f4f4fdd,0x5f5f5fdd,0x6f6f6fdd,0x7f7f7fdd},
    .a = {0x0f0f0faa,0x1f1f1faa,0x2f2f2faa,0x3f3f3faa,0x4f4f4faa,0x5f5f5faa,0x6f6f6faa},
    .usp = 0x12000,
    .pc = (uint32_t)user_routine_b,
    .next = &pid1_context,
  };
  pid1_context.next = &pid2_context;

  // Start the timer for firing the scheduler
  SET_VECTOR(context_swap, MFP_TIMER_C);

  // We need to set up USP before disabling supervisor mode
  // or we'll get a privilege error
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
  a0 = (uint32_t *)current_process->a[7];
  __asm__ __volatile__ ("move.l %%a0,%%usp":::);

  disable_supervisor();

  // We never return, but we also stop execution here after the
  // first context switch
  for (;;) {
    e68Print(".");
  }
}

/*
User space routine that doesn't do too much
*/
void user_routine_a() {
    for(;;) {
      e68DisplayNumUnsigned(1,10);
    }
}

/*
Nor this one
*/
void volatile user_routine_b() {
  for(;;) {
    e68DisplayNumUnsigned(2,10);
  }
}
