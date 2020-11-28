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
    bool running;       // temporary process state
} context_t;

typedef struct {
    uint16_t pid;       // Process ID
    state state;
    context_t context;
} process_t;

context_t *current_process;
context_t *process_a;
context_t *process_b;

void tick_handler();

void user_routine_a();
void user_routine_b();

noreturn void kmain() {
  debug_stub();

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  e68ClearScr();
  e68Println("Kernel started");

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  context_t context_a = {
    .usp = 0x4000,
    .pc = (uint32_t)user_routine_a,
    .sr = 0x0,
    .a = {0,0,0,0,0,0,0,0},
    .d = {0,0,0,0,0,0,0,0},
    .running = false
  };
  process_a = &context_a;

  context_t context_b = {
    .usp = 0x6000,
    .pc = (uint32_t)user_routine_b,
    .sr = 0x0,
    .a = {0,0,0,0,0,0,0,0},
    .d = {0,0,0,0,0,0,0,0},
    .running = false
  };
  process_b = &context_b;

  // Start the scheduler
  SET_VECTOR(tick_handler, MFP_TIMER_C);

  sleep();
}

/*
Interrupt handler for counting every time Timer C fires

-> SSP-6 Status Register
   SSP-4 Program Counter (High)
   SSP-2 Program Counter (Low)
   SSP (Pre-exception)

*/
void __attribute__ ((interrupt)) tick_handler() {
  // Should these just be globals in an include?

  //INC_LONG(SYS_TICKS);        // Count the ticks of the timer
  //e68Println("Handling tick");

  // Switch processes (this will be a circular linked list next)
  if (current_process == process_b) {
    current_process = process_a;
  } else {
    current_process = process_b;
  }

  // Context is meaningless if the process hasn't started
  if (current_process->running) {
    // Save PC (4B) and SR (2B) from stack into the old process context
    __asm__ __volatile__ (
      "move.w +2(%%a7),%0" : "=g" (current_process->pc) ::
    );
    
    // Save Address and Data registers to old process context
    __asm__ __volatile__ (
      "movem.l %%a0-%%a7/%%d0-%%d7,%0" : "=g" (current_process->a) ::
    );

    // Save USP to old process context
    __asm__ __volatile__ (
      "move %%usp,%%a0\n\t"
      "move %%a0,%0"
      :"=g" (current_process->usp)::
    );
  }

  e68Println(""); 
  e68Println("Next process context");
  e68Print("USP: 0x");
  e68DisplayNumUnsigned(current_process->usp,16);
  e68Println("");
  e68Print("PC: 0x");
  e68DisplayNumUnsigned(current_process->pc,16);
  e68Println("");
  e68Print("SR: 0x");
  e68DisplayNumUnsigned(current_process->sr,16);
  e68Println("");

  e68Print("A0:0x");
  e68DisplayNumUnsigned(current_process->a[0],16);
  e68Print(" A1:0x");
  e68DisplayNumUnsigned(current_process->a[1],16);
  e68Print(" A2:0x");
  e68DisplayNumUnsigned(current_process->a[2],16);
  e68Print(" A3:0x");
  e68DisplayNumUnsigned(current_process->a[3],16);
  e68Print(" A4:0x");
  e68DisplayNumUnsigned(current_process->a[4],16);
  e68Print(" A5:0x");
  e68DisplayNumUnsigned(current_process->a[5],16);
  e68Print(" A6:0x");
  e68DisplayNumUnsigned(current_process->a[6],16);
  e68Print(" A7:0x");
  e68DisplayNumUnsigned(current_process->a[7],16);
  e68Println("");
  
  e68Print("D0:0x");
  e68DisplayNumUnsigned(current_process->d[0],16);
  e68Print(" D1:0x");
  e68DisplayNumUnsigned(current_process->d[1],16);
  e68Print(" D2:0x");
  e68DisplayNumUnsigned(current_process->d[2],16);
  e68Print(" D3:0x");
  e68DisplayNumUnsigned(current_process->d[3],16);
  e68Print(" D4:0x");
  e68DisplayNumUnsigned(current_process->d[4],16);
  e68Print(" D5:0x");
  e68DisplayNumUnsigned(current_process->d[5],16);
  e68Print(" D6:0x");
  e68DisplayNumUnsigned(current_process->d[6],16);
  e68Print(" D7:0x");
  e68DisplayNumUnsigned(current_process->d[7],16);
  e68Println(""); 

  e68Print("Running: ");
  if (current_process->running) {
    e68Println("True");
  } else {
    e68Println("False");
  }

  // Move new USP into USP
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
  a0 = (uint32_t *)current_process->usp;
  __asm__ __volatile__ ("move %%a0,%%usp":::);

  //e68Print("USP set to:");
  //e68PrintNumSignedWidth(current_process->usp, 10);
  //e68Println("");

  // 4. Replace PC and SR on stack w/ new process's values
  // Program counter is at stack pointer+2
  __asm__ __volatile__ (
    "move.l %0,+2(%%a7)"  // I'm not convinced this offset is right
    ::"g" (current_process->pc):
  );

  //e68Print("PC set to:");
  //e68PrintNumSignedWidth(current_process->pc, 10);
  //e68Println("");

  // 6. Restore the A, D registers
  //__asm__ __volatile__ (
  //  "movem.l %0,%%a0-%%a7/%%d0-%%d7"
  //  ::"g" (current_process->a):
  //);
  current_process->running = true;
  // 7. Clear interrupt-in-service
  SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service

  // RTE implied by interrupt attribute on function
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