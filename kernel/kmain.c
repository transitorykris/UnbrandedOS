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

#include "easy68k/easy68k.h"
#include "system.h"

#define MFP_TIMER_C 0x114

typedef void (*fptr)();

void tick_handler();

noreturn void kmain() {
  fptr *handler = (fptr *)MFP_TIMER_C;
  *handler = &tick_handler;

  e68Println("Kernel started");

  for(;;){
    e68DisplayNumUnsigned(get_ticks(), 16);
    e68Println("");
  }
}

void __attribute__ ((interrupt)) tick_handler() {
  uint32_t *ticks = (uint32_t *)SYS_TICKS;
  (*ticks)++;
  __asm__ __volatile__ (
    "move.b #0xdf,0xf80011" // Clear MFP_ISRB interrupt-in-service
  );
}