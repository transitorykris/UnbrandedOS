/* kmain.c - Kris68k OS

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

#include <basicio.h>
#include <stdio.h>

typedef struct {
    int pid;
} process;

void process_a();
void process_b();

void kmain() {
    //printf("username: ");
    //char username[16];
    //readline(username, 16);
    //printf("Logging in %s\r\n", username);

    // - figure out how to catch timer interrupts?
    // - how does the stack look?
    // - how do I give each process it's own memory space?
    
    process procs[2];
    procs[0].pid = 0;
    procs[1].pid = 1;
    procs[2].pid = 2;

    printf("Process 0 PID is %d\r\n", procs[0].pid);
    printf("Process A PID is %d\r\n", procs[1].pid);
    printf("Process B PID is %d\r\n", procs[2].pid);

    process_a();
    
    // Kernel goes to sleep
    printf("Resetting system\r\n");
    //for(;;);
}

void process_a() {
    //for(;;) {
        printf("A\r\n");
    //}
}

void process_b() {
    for (;;) {
        printf("B\r\n");
    }
}