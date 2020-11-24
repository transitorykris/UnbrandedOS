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

#define TICK_HANDLER 0x114

typedef void (*fptr)();

uint16_t get_status_register();
void tick_handler();

noreturn void kmain() {
  fptr *handler = (fptr *)0x114;
  *handler = &tick_handler;

  e68Println("Kernel started");

  for(;;){
    e68DisplayNumUnsigned(get_ticks(), 16);
    e68Println("");
  }
}

uint16_t get_status_register() {
  uint16_t sr;
  __asm__ __volatile__ (
    "move.w %%sr,%0": "=r" (sr)
  );
  return sr;
}

void __attribute__ ((interrupt)) tick_handler() {
  __asm__ __volatile__ (
    "move.l %d0,-(%a7)\n\t"
    "move.l 0x40C,%d0\n\t"
    "add.l  #1,%d0\n\t"
    "move.l %d0,0x40C\n\t"
    "move.l (%a7)+,%d0\n\t"
    "move.b #0xdf,0xf80011"
  );
}