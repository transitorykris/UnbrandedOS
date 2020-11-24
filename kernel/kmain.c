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
#include "helpers.h"
#include "system.h"
#include "mfp.h"

void tick_handler();

noreturn void kmain() {
  SET_VECTOR(tick_handler, MFP_TIMER_C);

  e68ClearScr();
  e68Println("Kernel started");

  for(;;) {
    e68Print("Ticks: ");
    e68DisplayNumUnsigned(get_ticks(), 16);
    e68Println("");
  }
}

/*
Interrupt handler for counting every time Timer C fires
*/
void __attribute__ ((interrupt)) tick_handler() {
  INC_LONG(SYS_TICKS);        // Count the ticks of the timer
  SET_BYTE(~0x20, MFP_ISRB);  // Clear interrupt-in-service
}