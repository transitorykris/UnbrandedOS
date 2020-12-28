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

#include "helpers.h"
#include "context.h"
#include "system.h"
#include "mfp.h"

// Order matters! Append only
struct context_t {
    uint32_t d[8];      // Data registers D1-D7
    uint32_t a[8];      // Address registers A1-A6, a7 will be stored in usp
    uint16_t sr;        // Status register (and really just CCR)
    uint32_t pc;        // Program counter

    // Next process to run
    struct context_t* next;

    // Order shouldn't matter too much below this line
};

uint32_t scratch;

struct context_t *current_process;

uint32_t context_switches;

void tick_handler();

void user_routine_a();
void user_routine_b();

extern uint32_t test_process;

noreturn void kmain() {
  debug_stub();

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  e68ClearScr();
  e68Println("Kernel started");

  // pid0 will always be defined so the system has something to do  
  struct context_t pid0_context = {
    .a = {0,0,0,0,0,0,0,0x6000}
  };
  current_process = &pid0_context;

  struct context_t pid1_context = {
    .d = {1,2,3,4,5,6},
    .a = {0xFF,22,33,44,55,66,77,0x10000},
    .pc = (uint32_t)user_routine_a,
  };
  current_process->next = &pid1_context;
  //current_process->next = &pid0_context;

  struct context_t pid2_context = {
    .d[5] = 0x123456,
    .a = {0,0,0,0,0,0,0,0x12000},
    .pc = (uint32_t)user_routine_b,
    .next = &pid1_context,
  };
  pid1_context.next = &pid2_context;
  //pid2_context.next = &pid0_context;

  // Start the timer for firing the scheduler
  SET_VECTOR(context_swap, MFP_TIMER_C);

  // We need to set up USP before disabling supervisor mode
  // or we'll get a privilege error
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
  a0 = (uint32_t *)current_process->a[7];
  __asm__ __volatile__ ("move.l %%a0,%%usp":::);

  disable_supervisor();

  // pid0 will stop after the first context switch
  /*for (;;) {
    e68Print(".");
    e68Println("");
  }*/

  for (int i=0;;i++) {
    //e68Print("COUNTER:");
    //e68DisplayNumUnsigned(i,10);
    //e68Print("TICKS:");
    //e68DisplayNumUnsigned(get_ticks(),10);
    //e68Println(":END");
    e68Print(".");
    //e68ClearScr();
  }

  e68Println("Oh shit.");
}

/*
User space routine that doesn't do too much
*/
void user_routine_a() {
  for (int i=0;;i++) {
    e68Print("COUNTER:");
    e68DisplayNumUnsigned(i,10);
    e68Print(":TICK:");
    e68DisplayNumUnsigned(get_ticks(),10);
    e68Println(":END");
    //e68ClearScr();
  }
}

/*
Nor this one
*/
void user_routine_b() {
  for(;;) {
    //e68DisplayNumUnsigned(1234,10);
    e68Print("+");
  }
  /*for (;;) {
    e68Println("a");
    e68Println("b");
    e68Println("c");
    e68Println("d");
    e68Println("e");
    e68Println("f");
    e68Println("g");
    e68Println("h");
    e68Println("i");
    e68Println("j");
    e68Println("k");
    e68Println("l");
    e68Println("m");
    e68Println("n");
    e68Println("o");
    e68Println("p");
    e68Println("q");
    e68Println("r");
    e68Println("s");
    e68Println("t");
    e68Println("u");
    e68Println("v");
    e68Println("w");
    e68Println("x");
    e68Println("y");
    e68Println("z");
  }*/
}
