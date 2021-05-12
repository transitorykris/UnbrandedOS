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
#include "../unistd.h"
#include <machine.h>
#include <basicio.h>
#include <string.h>

#include "../sys/errors.h"
#include "../spawn.h"
#include "../wait.h"

#include "../fs/fs.h"

#include "shell.h"
#include "commands.h"

#define PROMPT  "/# "

struct commands_t {
    char *name;
    void (*func)();
};

void shell() {
    int count = 0;
    printf("Starting shell\n\n\r");
    
    // These are commands built into the shell
    struct commands_t commands[] = {
        {
            .name = "ls",
            .func = ls,
        },
    };
    int command_count = sizeof commands / sizeof (struct commands_t);

    // Big TODO
    // Tokenize the user's input and pass the arguments
    // to the new process
    for(;;) {
        printf(PROMPT);
        for (;;) {
            count = readline(buffer, BUFFER_LEN);
            printf("\n\r");
            if (count > 0) {
                // Check built in commands first
                for(int i=0;i<command_count;i++) {
                    if (!strcmp(buffer, commands[i].name)) {
                        commands[i].func();
                        goto done;
                    }
                }
                // Check our local directory next
                // XXX probably not how we want to do this (leaning on
                // exec to tell us if the executable exists..)
                // fork() somewhere here
                // NULL is required after last argument
                char *argv[MAX_ARGS] = {NULL};
                // TODO: parse buffer, for now, no arguments
                argv[0] = buffer;   // first argument is the process name
                // Attempt to spawn the process
                pid_t pid;
                int err = posix_spawn(&pid, buffer, NULL, NULL, argv, NULL);
                if (err == ERR_TOO_MANY_PROCS) {
                    printf("posix_spawn: too many processes\n\r");
                } else if (err == TOO_MANY_CHILDREN) {
                    printf("posix_spawn: too many children\n\r");
                } else {
                    int *stat_loc;
                    if(wait(stat_loc) == -1) {
                        printf("wait: error\n\r");
                    }
                }
            }
done:
            break;
        }
    }
}
