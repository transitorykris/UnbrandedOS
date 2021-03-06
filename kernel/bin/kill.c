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

#include "../signal.h"

#include "../musl/include/stdlib.h"

#include "../users.h"
#include "../process.h"
#include "../context.h"

/*
kill a process (really just put it to sleep for now..) 

argv[1] contains the pid
*/
int killer(int argc, char *argv[]) {
    if (argc != 2) {
        printf("%s: not enough arguments\n\r", argv[0]);
        return -1;
    }

    unsigned int pid = atoi(argv[1]);
    if (pid == 0 || pid >= MAX_PROCESSES) {
        printf("%d is not a valid PID\n\r");
        return -2;
    }

    kill(pid, SIGKILL);

    return 0;
}