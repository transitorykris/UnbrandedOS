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

typedef uint8_t state;
#define RUNNING 0
#define DEAD 1

typedef struct {
    uint32_t a[8];      // Address registers
    uint32_t d[8];      // Data registers
    uint32_t c[8];      // Address registers
    uint32_t e[8];      // Data registers
    uint16_t sr;        // Status register (and really just CCR)
    uint16_t pc_low;    // Program counter low word
    uint16_t pc_high;   // Program counter high word
    uint32_t usp;       // User stack pointer
    bool running;       // temporary process state
} context_t;

uint32_t scratch;

typedef struct {
    uint16_t pid;       // Process ID
    state state;
    context_t context;
} process_t;

context_t *current_process;
context_t *next_process;
context_t *process_0;
context_t *process_1;

uint32_t *spp;

void tick_handler();

void user_routine_a();
void pid0();

noreturn void kmain() {
  debug_stub();

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  e68ClearScr();
  e68Println("Kernel started");

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  /*context_t context_a = {
    .usp = 0x4000,
    .pc = (uint32_t)user_routine_a,
    .sr = 0x0,
    .a = {0,0,0,0,0,0,0,0},
    .d = {0,0,0,0,0,0,0,0},
    .running = false
  };
  process_a = &context_a;*/

  for (int i=0;i<50000;i++) {/* do nothing for a while */}
  
  // We need a process always running and we need to keep track of it
  // I guess this is like process 0 in Unix?
  // This will be populated by the scheduler's first context switch
  context_t pid0_context = {
    .usp = 0x6000,
    .pc_low = 0x0,
    .pc_high = 0x0,
    .sr = 0x0,
    .d = {0,0,0,0,0,0,0,0},
    .a = {0,0,0,0,0,0,0,0},
    .running = true
  };
  process_0 = &pid0_context;
  current_process = process_0;

  context_t process_1_c = {
    .usp = 0x8000,
    .pc_low = 0x0,
    .pc_high = 0x0,
    .sr = 0x0,
    .d = {0,0,0,0,0,0,0,0},
    .a = {0,0,0,0,0,0,0,0},
    .running = true
  };

  process_1 = &process_1_c;

  // Start the scheduler
  //SET_VECTOR(tick_handler, MFP_TIMER_C);
  SET_VECTOR(context_swap, MFP_TIMER_C);

  // We need to set up USP before disabling supervisor mode
  // or we'll get a privilege error
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
  a0 = (uint32_t *)current_process->usp;
  __asm__ __volatile__ ("move.l %%a0,%%usp":::);

  disable_supervisor();

  //unsigned int i=0;
  // We're effectively PID0 starting here
  while(true) {
    //e68DisplayNumUnsigned(i,10);
    //e68Println("");
    //i++;
    e68DisplayNumUnsigned((uint32_t)get_ticks(),10);
    e68Println("");
  }
  //while(true) i++;
}

/*
Interrupt handler for counting every time Timer C fires

-> SSP-6 Status Register
   SSP-4 Program Counter (High)
   SSP-2 Program Counter (Low)
   SSP (Pre-exception)

*/
/*void __attribute__ ((interrupt)) tick_handler() {
  // Save the registers to the stack

  __asm__ __volatile__ (
    "move.l %0,-(%%sp)"
    "move.l %1,-(%%sp)"
    ::"g" (current_process), "g" (next_process):
  );
  context_swap();

  SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service
*/
  /* Stack should look like
-> SSP-38 A7
   SSP-36 A6
   SSP-34 A5
   SSP-32 A4
   SSP-30 A3
   SSP-28 A2
   SSP-26 A1
   SSP-24 A0
   SSP-22 D7
   SSP-20 D6
   SSP-18 D5
   SSP-16 D4
   SSP-14 D3
   SSP-12 D2
   SSP-10 D1
   SSP-8  D0
   SSP-6  Status Register
   SSP-4  Program Counter (High)
   SSP-2  Program Counter (Low)
   SSP    (Pre-exception)
  */

  // We're going to save and restore everything
  // We already get the program count and status register saved
  // on the stack

  // Point a7 at our context struct
  //__asm__ __volatile__ ("move.l %0, %%a7"::"g" (current_process->c):"%a7");
  //__asm__ __volatile__ ("movem.l  %d0-%d7/%a0-%a6,-(%a7)");
  //__asm__ __volatile__ ("move.l %%a7,%0":"=g" (scratch):);
  //68Print("T");
  /*e68Print("a7:");
  e68DisplayNumUnsigned(current_process->a[7],16);
  e68Print(" a6:");
  e68DisplayNumUnsigned(current_process->a[6],16);
  e68Print(" a5:");
  e68DisplayNumUnsigned(current_process->a[5],16);
  e68Print(" a4:");
  e68DisplayNumUnsigned(current_process->a[4],16);
  e68Print(" a3:");
  e68DisplayNumUnsigned(current_process->a[3],16);
  e68Print(" a2:");
  e68DisplayNumUnsigned(current_process->a[2],16);
  e68Print(" a1:");
  e68DisplayNumUnsigned(current_process->a[1],16);
  e68Print(" a0:");
  e68DisplayNumUnsigned(current_process->a[0],16);
  e68Println("");
  e68Print("d7:");
  e68DisplayNumUnsigned(current_process->d[7],16);
  e68Print(" d6:");
  e68DisplayNumUnsigned(current_process->d[6],16);
  e68Print(" d5:");
  e68DisplayNumUnsigned(current_process->d[5],16);
  e68Print(" d4:");
  e68DisplayNumUnsigned(current_process->d[4],16);
  e68Print(" d3:");
  e68DisplayNumUnsigned(current_process->d[3],16);
  e68Print(" d2:");
  e68DisplayNumUnsigned(current_process->d[2],16);
  e68Print(" d1:");
  e68DisplayNumUnsigned(current_process->d[1],16);
  e68Print(" d0:");
  e68DisplayNumUnsigned(current_process->d[0],16);
  e68Println("");*/


  // Restore
  //__asm__ __volatile__ ("move.l %0, %%a7"::"g" (scratch):"%a7");
  //__asm__ __volatile__ ("movem.l  (%a7)+,%a6-%a0/%d7-%d0":::"a6","a5","a4","a3","a2","a1","a0","d7","d6","d5","d4","d3","d2","d1","d0");

  //e68DisplayNumUnsigned(current_process->pc_high, 16);
  //e68Println("");

  //__asm__ __volatile__ (
  //  "move.w (%sp)+,%sp\n\t"
  //  "move.w (%sp)+,%sp"
  //);

  //SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service

  //__asm__ __volatile__ (
  //  "move.l (%sp),%sp"
  //);

  // Point current_process at the stack pointer
  //__asm__ __volatile__ (
  //  "move %%sp,%0"
  //  :"=g" (spp)::
  //);

  // Print out each element
  // current_process->* should be a SSP pointer to decrement
  /*e68Println("");
  e68Print("A0:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A1:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A2:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A3:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A4:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A5:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A6:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" A7:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Println("");
  e68Print("D0:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D1:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D2:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D3:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D4:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D5:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D6:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" D7:0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Println(""); 
  e68Print("USP: 0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Println("");
  e68Print("PC low: 0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Print(" PC high: 0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Println("");
  e68Print("SR: 0x");
  e68DisplayNumUnsigned(*spp++,16);
  e68Println("");*/
  
  // Resetting the interrupt happens further down

  // Should these just be globals in an include?

  //INC_LONG(SYS_TICKS);        // Count the ticks of the timer
  //e68Println("Handling tick");

  // Switch processes (this will be a circular linked list next)
  //if (current_process == process_b) {
  //  current_process = process_a;
  //} else {
  //  current_process = process_b;
  //}

  /*// Context is meaningless if the process hasn't started
  if (current_process->running) {
    // Save Address and Data registers to old process context
    __asm__ __volatile__ (
      "movem.l %%d0-%%d7,%0"
      :"=g" (current_process->d)::
    );

    __asm__ __volatile__ (
      "movem.l %%a0-%%a7,%0"
      :"=g" (current_process->a)::
    );
    
    // Save the current process's SR
    __asm__ __volatile__ (
      "move.l %%sp@(6),%0"
      :"=g" (current_process->sr)::
    );

    // Save the current process's PC
    __asm__ __volatile__ (
      "move.w %%sp@(2),%0\n\t"
      "move.w %%sp@(4),%1"
      : "=g" (current_process->pc_low), "=g" (current_process->pc_high)::
    );

    // Save USP to old process context
    __asm__ __volatile__ (
      "move.l %%usp,%%a0\n\t"
      "move.l %%a0,%0"
      :"=g" (current_process->usp)::
    );
  }

  e68Println(""); 
  e68Print("USP: 0x");
  e68DisplayNumUnsigned(current_process->usp,16);
  e68Println("");
  e68Print("PC low: 0x");
  e68DisplayNumUnsigned(current_process->pc_low,16);
  e68Print(" PC high: 0x");
  e68DisplayNumUnsigned(current_process->pc_high,16);
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

  current_process->running = true;

  // Move next USP into USP
  register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
  a0 = (uint32_t *)current_process->usp;
  __asm__ __volatile__ ("move.l %%a0,%%usp":::);

  // Replace PC w/ next process's values
  __asm__ __volatile__ (
    //"move.w %0,+2(%%a7)\n\t"  // I'm not convinced this offset is right
    //"move.w %1,+4(%%a7)"
    "move.w %0,%%sp@(2)\n\t"  // I'm not convinced this offset is right
    "move.w %1,%%sp@(4)"
    ::"g" (current_process->pc_low), "g" (current_process->pc_high):
  );

  // Replace SR w/ next process's values
  __asm__ __volatile__ (
    "move.l %0,(%%sp)"
    ::"g" (current_process->sr):
  );*/

  // Clear interrupt-in-service
  //SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service

  /*// Restore the A, D registers
  __asm__ __volatile__ (
    "movem.l (%0),%%d0-%%d7/%%a0-%%a7"
    ::"g" (current_process->d):
  );

  //__asm__ __volatile__ (
  //  "movem.l (%0),%%a0-%%a7"
  //  ::"g" (current_process->a):
  //);*/

  // RTE implied by interrupt attribute on function
//}

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
void pid0() {
  //__asm__ __volatile (
  //  "move.l #0xf0f0f0f0,%d4"
  //);
  //e68Println("PID0 sleep forever");
  for (;;) {
    e68Println("."); // this will go away once things work
  }
}