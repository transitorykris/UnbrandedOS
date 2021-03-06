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
#include "../../sys/types.h"
#include "../../unistd/unistd.h"

#include "../../fs/fs.h"

#include "commands.h"

/* List files in the root directory */
void ls() {
    for (int i=0;i<MAX_FILES;i++) {
        char *filename = fs.root->files[i].name;
        if (filename) {
            printf("%s\n\r", filename);
        }
    }
}

/* Print out the current user's uid */
void shell_getuid() {
    printf("%d\n\r", getuid());
}

/* Print the current working directory */
void shell_pwd() {
    char buf[MAX_FILE_NAME_LEN];
    printf("%s\n\r", getcwd(buf, MAX_FILE_NAME_LEN));
}