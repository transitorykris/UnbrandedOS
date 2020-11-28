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

void save_context(context_t *context);
void set_context(context_t *context);

void tick_handler();

void user_routine_a();
void user_routine_b();

noreturn void kmain() {
  debug_stub();

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  SET_VECTOR(tick_handler, MFP_TIMER_C);

  e68ClearScr();
  e68Println("Kernel started");

  for (int i=0;i<50000;i++) {/* do nothing for a while */}

  context_t context_a = {
    .usp = 0x4000,
    .pc = (uint32_t)user_routine_a,
    .sr = 0x0
  };
  set_context(&context_a);
  disable_supervisor();
  __asm__ __volatile (
    "move.w %0,%%a0\n\t"
    "jmp (%%a0)"
    : : "g" (context_a.pc) :
  );

  sleep();
}

/*
Save the current process's context
*/
void save_context(context_t *context) {

}

/*
Set up the context for a new process
*/
void set_context(context_t *context) {
    register uint32_t *a0 __asm__ ("a0") __attribute__((unused));

    // Set the user stack pointer
    a0 = (uint32_t *)context->usp;
    __asm__ __volatile__ (
        "move %%a0,%%usp":::"a0"
    );
}

/*
Interrupt handler for counting every time Timer C fires
*/
void __attribute__ ((interrupt)) tick_handler() {
  INC_LONG(SYS_TICKS);        // Count the ticks of the timer

  // Scheduler
  // 1. Save PC (4B) and SR (2B) from stack into the old process context
  // 2. Save USP to old process context
  // 3. Save Address and Data registers to old process context
  // 4. Replace PC and SR on stack w/ new process's values
  // 5. Move new USP into USP
  // 6. Restore the A, D registers
  // 7. Clear interrupt-in-service
  // 8. RTE

  SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service
}

/*
User space routine that doesn't do too much
*/
void user_routine_a() {
  for (;;) {
    e68Println("A");
  }
}

/*
User space routine that doesn't do too much
Same as the first
*/
void user_routine_b() {
  for (;;) {
    e68Println("B");
  }
}