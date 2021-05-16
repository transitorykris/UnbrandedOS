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

#ifndef SPAWN_H
#define SPAWN_H

#include "process.h"

#define FILE_NOT_FOUND      -3
#define TOO_MANY_CHILDREN   -2

// To be implemented
typedef void posix_spawn_file_actions_t;
typedef void posix_spawnattr_t;

// https://pubs.opengroup.org/onlinepubs/009695399/functions/posix_spawn.html

int posix_spawn(pid_t *restrict pid, const char *restrict path,
       const posix_spawn_file_actions_t *file_actions,
       const posix_spawnattr_t *restrict attrp,
       char *const argv[restrict], char *const envp[restrict]);

#endif