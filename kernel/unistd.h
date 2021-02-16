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

#ifndef UNISTD_H
#define UNISTD_H

#include "process.h"

//#define STDIN_FILENO    0
//#define STDOUT_FILENO   1
//#define STDERR_FILENO   2

// https://pubs.opengroup.org/onlinepubs/9699919799/

int execvp(const char *file, char *const argv[]);
void _exit(int status);
pid_t fork(void);

//int access(const char *, int);
//unsigned alarm(unsigned);
//int chdir(const char *);
//int chown(const char *, uid_t, gid_t);
//int close(int);
//size_t confstr(int, char *, size_t);
//char *crypt(const char *, const char *);
//int dup(int);
//int dup2(int, int);
//void encrypt(char [64], int);
//int execl(const char *, const char *, ...);
//int execle(const char *, const char *, ...);
//int execlp(const char *, const char *, ...);
//int execv(const char *, char *const []);
//int execve(const char *, char *const [], char *const []);
//int faccessat(int, const char *, int, int);
//int fchdir(int);
//int fchown(int, uid_t, gid_t);
//int fchownat(int, const char *, uid_t, gid_t, int);
//int fdatasync(int);
//int fexecve(int, char *const [], char *const []);
//long fpathconf(int, int);
//int fsync(int);
//int ftruncate(int, off_t);
//char *getcwd(char *, size_t);
//gid_t getegid(void);
//uid_t geteuid(void);
//gid_t getgid(void);
//int getgroups(int, gid_t []);
//long gethostid(void);
//int gethostname(char *, size_t);
//char *getlogin(void);
//int getlogin_r(char *, size_t);
//int getopt(int, char * const [], const char *);
//pid_t getpgid(pid_t);
//pid_t getpgrp(void);
//pid_t getpid(void);
//pid_t getppid(void);
//pid_t getsid(pid_t);
//uid_t getuid(void);
//int isatty(int);
//int lchown(const char *, uid_t, gid_t);
//int link(const char *, const char *);
//int linkat(int, const char *, int, const char *, int);
//int lockf(int, int, off_t);
//off_t lseek(int, off_t, int);
//int nice(int);
//long pathconf(const char *, int);
//int pause(void);
//int pipe(int [2]);
//ssize_t pread(int, void *, size_t, off_t);
//ssize_t pwrite(int, const void *, size_t, off_t);
//ssize_t read(int, void *, size_t);
//ssize_t readlink(const char *restrict, char *restrict, size_t);
//ssize_t readlinkat(int, const char *restrict, char *restrict, size_t);
//int rmdir(const char *);
//int setegid(gid_t);
//int seteuid(uid_t);
//int setgid(gid_t);
//int setpgid(pid_t, pid_t);
//pid_t setpgrp(void);
//int setregid(gid_t, gid_t);
//int setreuid(uid_t, uid_t);
//pid_t setsid(void);
//int setuid(uid_t);
//unsigned sleep(unsigned);
//void swab(const void *restrict, void *restrict, ssize_t);
//int symlink(const char *, const char *);
//int symlinkat(const char *, int, const char *);
//void sync(void);
//long sysconf(int);
//pid_t tcgetpgrp(int);
//int tcsetpgrp(int, pid_t);
//int truncate(const char *, off_t);
//char *ttyname(int);
//int ttyname_r(int, char *, size_t);
//int unlink(const char *);
//int unlinkat(int, const char *, int);
//ssize_t write(int, const void *, size_t);

#endif