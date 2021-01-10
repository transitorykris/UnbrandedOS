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

// Fiddling around with memory allocation

#include <stdio.h>

#include "../malloc.h"

int memplay(int argc, char *argv[]) {
    char *a_string;

    a_string = malloc(15);  // hello world!\n\r

    printf("a_string @ %d", a_string);

    a_string[0] = 'h';
    a_string[1] = 'e';
    a_string[2] = 'l';
    a_string[3] = 'l';
    a_string[4] = 'o';
    a_string[5] = ' ';
    a_string[6] = 'w';
    a_string[7] = 'o';
    a_string[8] = 'r';
    a_string[9] = 'l';
    a_string[10] = 'd';
    a_string[11] = '!';
    a_string[12] = '\n';
    a_string[13] = '\r';
    a_string[14] = '\0';

    printf("-> %s", a_string);

    // B

    char *b_string;

    b_string = malloc(15);  // hello world!\n\r

    printf("a_string @ %d", a_string);

    b_string[0] = 's';
    b_string[1] = 'o';
    b_string[2] = ' ';
    b_string[3] = 'l';
    b_string[4] = 'o';
    b_string[5] = 'n';
    b_string[6] = 'g';
    b_string[7] = ' ';
    b_string[8] = 'y';
    b_string[9] = 'o';
    b_string[10] = 'u';
    b_string[11] = '!';
    b_string[12] = '\n';
    b_string[13] = '\r';
    b_string[14] = '\0';

    printf("-> %s", b_string);

    // Back to A

    printf("-> %s", a_string);

    return 0;
}