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

/*
blinky blinks an LED on the rosco-m68k
*/

#include <stdio.h>
#include <gpio.h>
#include <machine.h>

#include "../process.h"
#include "../context.h"

void _blinky();

int blinkd(int argc, char *argv[]) {
    printf("Starting %s\n\r", argv[0]);
    
    // The world's most inaesthetic daemonization
    //create_process(&blinkd_ctx, argv[0], (uint32_t)_blinky);
    create_process(argv[0], (uint32_t)_blinky, 0);

    return 0;
}

void _blinky() {
    for (int i=0;;i++) {
        if (i%1000 == 0) {
            digitalWrite(led_green, !digitalRead(led_green));
        }
    }
}