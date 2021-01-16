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

#include "helpers.h"
#include "system.h"

/*
Returns the number of ticks since the system started
*/
tick_t get_ticks() {
    return GET_LONG(SYS_TICKS);
}

/*
Returns the value of the status register
*/
uint16_t get_status_register() {
    uint16_t sr;
    __asm__ __volatile__ (
        "move.w %%sr,%0": "=r" (sr)
    );
    return sr;
}

/*
Set the user stack pointer
*/
__attribute__((gnu_inline)) void inline set_usp(uint32_t usp) {
    register uint32_t *a0 __asm__ ("a0") __attribute__((unused));
    a0 = (uint32_t *)usp;
    __asm__ __volatile__ (
        "move %%a0,%%usp":::"a0"
    );
}

/*
Delays for some number of duration units
*/
void delay(uint32_t duration) {
  for (int i=0;i<duration;i++);
}