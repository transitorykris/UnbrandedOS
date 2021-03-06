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

#ifndef SYSTEM_H
#define SYSTEM_H

#define SYS_TICKS 0x40C

#include "sys/types.h"

/*
Disable supervisor bit
*/
#define disable_supervisor() \
{\
    __asm__ __volatile__ ("andi.w #0xdfff,%sr");\
}

/*
Disable interrupts
*/
#define disable_interrupts()                    \
{                                               \
    __asm__ __volatile__ ("ori.w #0x700,%sr");  \
}

/*
Enable interrupts
*/
#define enable_interrupts()                     \
{                                               \
    __asm__ __volatile__ ("andi.w #0xF8FF,%sr"); \
}

/*
Sleep... forever
*/
#define sleep() for(;;);

typedef unsigned int tick_t;

unsigned int get_ticks();
uint16_t get_status_register();
__attribute__((gnu_inline)) void inline set_usp(uint32_t *usp);
void delay(uint32_t duration);

#endif