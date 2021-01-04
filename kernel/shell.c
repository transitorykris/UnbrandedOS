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
#include "process.h"
#include "context.h"

#include "shell.h"

#define PROMPT  "/# "

void exec(void (*func)());
void uptime();
void ps();

struct commands_t {
    char *name;
    void (*func)();
};

void shell() {
    int count = 0;
    printf("Starting shell\n\n\r");
    
    struct commands_t commands[] = {
        {
            .name = "uptime",
            .func = uptime,
        },
        {
            .name = "ps",
            .func = ps,
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
                        exec(commands[i].func);
                        goto done;
                    }
                }
                printf("command not found: %s\n\r", buffer);
            }
done:
            break;
        }
    }
}

/* A pretty bad exec function */
void exec(void (*func)()) {
    func();
}

/* A tiny program that prints the number of ticks */
void uptime() {
    uint32_t ticks = get_ticks();
    uint32_t seconds = ticks / 100;
    printf("up %d seconds, 1 user\n\r", seconds);
}

/* Print out all defined processes */
void ps() {
    printf("%s %18s %20s\n\r", "PID", "CMD", "State");
    for(int i=0;i<MAX_PROCESSES;i++) {
        if (processes[i].name != NULL) {
            printf("%d %20s %20s\n\r",
                i, 
                processes[i].name, 
                process_state(processes[i].context->state)
            );
        }
    }
}