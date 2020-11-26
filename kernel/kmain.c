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

#include "easy68k/easy68k.h"

#include "helpers.h"
#include "system.h"
#include "mfp.h"
#include "scheduler.h"

void tick_handler();

void user_routine_a();
void user_routine_b();

struct process_item {
    process* process;
    struct process_item* next;
} process_item;

struct process_item *process_list;

noreturn void kmain() {
  SET_VECTOR(tick_handler, MFP_TIMER_C);

  e68ClearScr();
  e68Println("Kernel started");

  // Rig up our process list by hand. Sorry.
  process process_a = {.pid=0, .context= {.usp=0, .pc=0, .sr=0}};
  process process_b = {.pid=1, .context= {.usp=0, .pc=0, .sr=0}};
  
  process_list->process = &process_a;
  
  struct process_item *process_list_b = \
    (struct process_item*)malloc(sizeof(process_item));

  process_list_b->process = &process_b;
  process_list_b->next = process_list;
  
  process_list->next = process_list_b;

  disable_supervisor();
  user_routine_a();

  for(;;);
}

/*
Interrupt handler for counting every time Timer C fires
*/
void __attribute__ ((interrupt)) tick_handler() {
  INC_LONG(SYS_TICKS);        // Count the ticks of the timer

  process_list = process_list->next;
  e68PrintNumSignedWidth(process_list->process->pid, 16);
  e68Println("");

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