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
#include "system.h"
#include "mfp.h"

typedef uint8_t state;
#define RUNNING 0
#define DEAD 1

typedef struct {
    uint32_t usp;       // User stack pointer
    uint32_t pc;        // Program counter
    uint16_t sr;        // Status register (and really just CCR)
    uint32_t a[8];      // Address registers, 8 is usp
    uint32_t d[8];      // Data registers
} context_t;

typedef struct {
    uint16_t pid;       // Process ID
    state state;
    context_t context;
} process_t;

int process_list;
context_t *current_process;
context_t *process_a;
context_t *process_b;
bool ready;

void tick_handler();

void user_routine_a();
void user_routine_b();

noreturn void kmain() {
  debug_stub();

  ready = false;

  process_list = 0; // 0 = A, 1 = B

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  e68ClearScr();
  e68Println("Kernel started");

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  context_t context_a = {
    .usp = 0x4000,
    .pc = (uint32_t)user_routine_a,
    .sr = 0x0
  };
  process_a = &context_a;

  context_t context_b = {
    .usp = 0x6000,
    .pc = (uint32_t)user_routine_b,
    .sr = 0x0
  };
  process_b = &context_b;

  // Start the scheduler
  SET_VECTOR(tick_handler, MFP_TIMER_C);

  //disable_supervisor();

  sleep();
}

/*
Interrupt handler for counting every time Timer C fires
*/
void __attribute__ ((interrupt)) tick_handler() {
  INC_LONG(SYS_TICKS);        // Count the ticks of the timer
  //e68Println("Handling tick");

  if (ready) {
    // 1. Save PC (4B) and SR (2B) from stack into the old process context
    __asm__ __volatile__ (
      "move +2(%%a7),%0"
      :"=g" (current_process->pc)::
    );

    //e68PrintNumSignedWidth(current_process->pc, 10);
    //e68Println("");
    
    // 3. Save Address and Data registers to old process context
    __asm__ __volatile__ (
      "move %%a0,%0\n\t"
      "move %%a1,%1\n\t"
      "move %%a2,%2\n\t"
      "move %%a3,%3\n\t"
      "move %%a4,%4\n\t"
      "move %%a5,%5\n\t"
      "move %%a6,%6\n\t"
      "move %%a7,%6"
      :
      "=g" (current_process->a[0]),
      "=g" (current_process->a[1]),
      "=g" (current_process->a[2]),
      "=g" (current_process->a[3]),
      "=g" (current_process->a[4]),
      "=g" (current_process->a[5]),
      "=g" (current_process->a[6]),
      "=g" (current_process->a[0])
      ::
    );

    //e68PrintNumSignedWidth(current_process->a[0], 10);
    //e68Println("");

    __asm__ __volatile__ (
      "move %%d0,%0\n\t"
      "move %%d1,%1\n\t"
      "move %%d2,%2\n\t"
      "move %%d3,%3\n\t"
      "move %%d4,%4\n\t"
      "move %%d5,%5\n\t"
      "move %%d6,%6\n\t"
      "move %%d7,%6"
      :
      "=g" (current_process->d[0]),
      "=g" (current_process->d[1]),
      "=g" (current_process->d[2]),
      "=g" (current_process->d[3]),
      "=g" (current_process->d[4]),
      "=g" (current_process->d[5]),
      "=g" (current_process->d[6]),
      "=g" (current_process->d[0])
      ::
    );

    // 2. Save USP to old process context
    __asm__ __volatile__ (
      "move %%usp,%%a0\n\t"
      "move %%a0,%0"
      :"=g" (current_process->usp)::
    );
  }

  //e68PrintNumSignedWidth(current_process->usp, 10);
  //e68Println("");

  // Switch processes (this will be a circular linked list next)
  if (process_list == 1) {
    current_process = process_a;
    process_list = 0;
    ready = true; // temporary hack
    e68Println("Switching to process_a");
  } else {
    process_list = 1;
    current_process = process_b;
    ready = true; // temporary hack
    e68Println("Switching to process_b");
  }

  // 5. Move new USP into USP
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));

  a0 = (uint32_t *)current_process->usp;
  __asm__ __volatile__ (
    "move %%a0,%%usp"
    :::
  );

  e68Println("User Stack Pointer set");

  // 4. Replace PC and SR on stack w/ new process's values
  // Program counter is at stack pointer+2
  __asm__ __volatile__ (
    "move.l %0,2(%%a7)"
    ::"g" (current_process->pc):
  );

  e68Println("Program Counter set");

  // 6. Restore the A, D registers
  __asm__ __volatile__ (
    "move.w %0,%%a0\n\t"
    "move.w %1,%%a1\n\t"
    "move.w %2,%%a2\n\t"
    "move.w %3,%%a3\n\t"
    "move.w %4,%%a4\n\t"
    "move.w %5,%%a5\n\t"
    "move.w %6,%%a6\n\t"
    "move.w %7,%%a7\n\t"
    ::
    "g" (current_process->a[0]),
    "g" (current_process->a[1]),
    "g" (current_process->a[2]),
    "g" (current_process->a[3]),
    "g" (current_process->a[4]),
    "g" (current_process->a[5]),
    "g" (current_process->a[6]),
    "g" (current_process->a[7])
    :
  );

  __asm__ __volatile__ (
    "move.w %0,%%d0\n\t"
    "move.w %1,%%d1\n\t"
    "move.w %2,%%d2\n\t"
    "move.w %3,%%d3\n\t"
    "move.w %4,%%d4\n\t"
    "move.w %5,%%d5\n\t"
    "move.w %6,%%d6\n\t"
    "move.w %7,%%d7\n\t"
    ::
    "g" (current_process->d[0]),
    "g" (current_process->d[1]),
    "g" (current_process->d[2]),
    "g" (current_process->d[3]),
    "g" (current_process->d[4]),
    "g" (current_process->d[5]),
    "g" (current_process->d[6]),
    "g" (current_process->d[7])
    :
  );

  // 7. Clear interrupt-in-service
  SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service

  // 8. RTE
}

/*
User space routine that doesn't do too much
*/
void user_routine_a() {
  e68Println("Start of routine_a");
  for (;;) {
    e68Println("A");
  }
}

/*
User space routine that doesn't do too much
Same as the first
*/
void user_routine_b() {
  e68Println("Start of routine_b");
  for (;;) {
    e68Println("B");
  }
}