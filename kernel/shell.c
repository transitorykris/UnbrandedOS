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

#include <stdio.h>
#include <machine.h>
#include <basicio.h>
#include <string.h>
#include <stdlib.h>

#include "system.h"

#include "shell.h"

#define PROMPT  "/# "

void uptime();

struct commands_t {
    char *name;
    void (*func)();
};

void shell() {
    int count = 0;
    printf("Starting shell\n\r");
    
    struct commands_t commands[] = {
        {
            .name = "uptime",
            .func = uptime,
        }
    };
    int command_count = sizeof commands / sizeof (struct commands_t);

    for(;;) {
        printf(PROMPT);
        for (;;) {
            count = readline(buffer, BUFFER_LEN);
            printf("\n\r");
            if (count > 0) {
                for(int i=0;i<command_count;i++) {
                    if (!strcmp(buffer, commands[i].name)) {
                        commands[i].func();
                        break;
                    }
                    printf("command not found: %s\n\r", buffer);
                }
            }
            break;
        }
    }
}

/* A tiny program that prints the number of ticks */
void uptime() {
    uint32_t ticks = get_ticks();
    uint32_t seconds = ticks / 100;
    printf("up %d seconds, 1 user\n\r", seconds);
}
