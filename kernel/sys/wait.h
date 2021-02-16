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

#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include "types.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/

// waitpid() options
#define WCONTINUED  1   // Report status of continued child process.
#define WNOHANG     2   // Do not hang if no status is available; return immediately.
#define WUNTRACED   4   // Report status of stopped child process.

// waitpid() status
#define WEXITSTATUS     0   // Return exit status.
#define WIFCONTINUED    1   // True if child has been continued.
#define WIFEXITED       2   // True if child exited normally.
#define WIFSIGNALED     4   // True if child exited due to uncaught signal.
#define WIFSTOPPED      8   // True if child is currently stopped.
#define WSTOPSIG        16  // Return signal number that caused process to stop.
#define WTERMSIG        32  // Return signal number that caused process to terminate.

pid_t wait(int *stat_loc);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

// Does any system other than linux use this?
//WEXITED // Wait for processes that have exited.
//WNOWAIT // Keep the process whose status is returned in infop in a waitable state.
//WSTOPPED // Status is returned for any child that has stopped upon receipt of a signal.
//int waitid(idtype_t, id_t, siginfo_t *, int);

#endif