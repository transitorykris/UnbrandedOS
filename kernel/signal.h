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

#ifndef SIGNAL_H
#define SIGNAL_H

#include "sys/types.h"

#define SIGNAL_COUNT    29  // for allocating the signal handler table

// https://pubs.opengroup.org/onlinepubs/9699919799/

#define SIGNULL     0   // A placeholder because I miscounted
#define SIGABRT     1   // A Process abort signal.
#define SIGALRM     2   // T Alarm clock.
#define SIGBUS      3   // A Access to an undefined portion of a memory object.
#define SIGCHLD     4   // I Child process terminated, stopped,
#define SIGCONT     5   // C Continue executing, if stopped.
#define SIGFP       6   // A Erroneous arithmetic operation.
#define SIGHUP      7   // T Hangup.
#define SIGILL      8   // A Illegal instruction.
#define SIGINT      9   // T Terminal interrupt signal.
#define SIGKILL     10  // T Kill (cannot be caught or ignored).
#define SIGPIPE     11  // T Write on a pipe with no one to read it.
#define SIGQUIT     12  // A Terminal quit signal.
#define SIGSEGV     13  // A Invalid memory reference.
#define SIGSTOP     14  // S Stop executing (cannot be caught or ignored).
#define SIGTERM     15  // T Termination signal.
#define SIGTSTP     16  // S Terminal stop signal.
#define SIGTTIN     17  // S Background process attempting read.
#define SIGTTOU     18  // S Background process attempting write.
#define SIGUSR1     19  // T User-defined signal 1.
#define SIGUSR2     20  // T User-defined signal 2.
#define SIGPOLL     21  // T Pollable event. [Option End]
#define SIGPROF     22  // T Profiling timer expired. [Option End]
#define SIGSYS      23  // A Bad system call. [Option End]
#define SIGTRAP     24  // A Trace/breakpoint trap. [Option End]
#define SIGURG      25  // I High bandwidth data is available at a socket.
#define SIGVTALRM   26  // T Virtual timer expired.
#define SIGXCPU     27  // A CPU time limit exceeded.
#define SIGXFSZ     28  // A File size limit exceeded.

// A first crappy signal handling implmentation
typedef void (*signal_handler_t)();

void default_abnormal_handler(void);
void default_ignore_handler(void);
void default_stop_handler(void);
void default_continue_handler(void);

int kill(pid_t pid, int sig);

//int killpg(pid_t, int);
//void psiginfo(const siginfo_t *, const char *);
//void psignal(int, const char *);
//int pthread_kill(pthread_t, int);
//int pthread_sigmask(int, const sigset_t *restrict,
//         sigset_t *restrict);
//int raise(int);
//int sigaction(int, const struct sigaction *restrict,
//         struct sigaction *restrict);
//int sigaddset(sigset_t *, int);
//int sigaltstack(const stack_t *restrict, stack_t *restrict);
//int sigdelset(sigset_t *, int);
//int sigemptyset(sigset_t *);
//int sigfillset(sigset_t *);
//int sighold(int);
//int sigignore(int);
//int siginterrupt(int, int);
//int sigismember(const sigset_t *, int);
//void (*signal(int, void (*)(int)))(int);
//int sigpause(int);
//int sigpending(sigset_t *);
//int sigprocmask(int, const sigset_t *restrict, sigset_t *restrict);
//int sigqueue(pid_t, int, union sigval);
//int sigrelse(int);
//void (*sigset(int, void (*)(int)))(int);
//int sigsuspend(const sigset_t *);
//int sigtimedwait(const sigset_t *restrict, siginfo_t *restrict,
//         const struct timespec *restrict);
//int sigwait(const sigset_t *restrict, int *restrict);
//int sigwaitinfo(const sigset_t *restrict, siginfo_t *restrict);

#endif