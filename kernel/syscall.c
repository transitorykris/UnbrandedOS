/*
Copyright 2021 Kris Foster

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

#include <stdint.h>
#include <stdio.h>

#include "machine.h"

#include "system.h"
#include "syscall.h"
#include "process.h"
#include "fork.h"

inline uint32_t syscall(uint16_t num) {
    __asm__ __volatile__ (
        "move.l %0,%d0\n\t"
        "trap #0\n\t"       // syscalls are handled at trap 0
        :: "r" (num)
    );
}

/*__attribute__((interrupt)) void syscall_handler(void)  {
    disable_interrupts();
    // we get the syscall in d0 because user processes shouldn't
    // be monkeying with the supervisor's stack
    register uint32_t d0 __asm__ ("d0");
    uint32_t num = d0;
    switch(num) {
        case FORK:
            fork_handler();
        case REBOOT:
            _WARM_BOOT();
        default:
            break;
    }
    if (d0 == FORK) {
        fork_handler();
    }
    printf("finished handling syscall\n\r");
    //return -1;
}*/
