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

#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <stdint.h>

// https://pubs.opengroup.org/onlinepubs/9699919799/

typedef uint16_t pid_t; // Used for process IDs and process group IDs.
typedef uint16_t uid_t; // Used for user IDs.

//blkcnt_t              // Used for file block counts
//blksize_t             // Used for block sizes
//clock_t               // Used for system times in clock ticks or CLOCKS_PER_SEC (see <time.h>).
//clockid_t             // Used for clock ID type in the clock and timer functions.
//dev_t                 // Used for device IDs.
//fsblkcnt_t            // Used for file system block counts
//fsfilcnt_t            // Used for file system file counts
//gid_t                 // Used for group IDs.
//id_t                  // Used as a general identifier; can be used to contain at least a pid_t, uid_t or a gid_t.
//ino_t                 // Used for file serial numbers.
//key_t                 // Used for interprocess communication.
//mode_t                // Used for some file attributes.
//nlink_t               // Used for link counts.
//off_t                 // Used for file sizes.
//pthread_attr_t        // Used to identify a thread attribute object.
//pthread_cond_t        // Used for condition variables.
//pthread_condattr_t    // Used to identify a condition attribute object.
//pthread_key_t         // Used for thread-specific data keys.
//pthread_mutex_t       // Used for mutexes.
//pthread_mutexattr_t   // Used to identify a mutex attribute object.
//pthread_once_t        // Used for dynamic package initialisation.
//pthread_rwlock_t      // Used for read-write locks.
//pthread_rwlockattr_t  // Used for read-write lock attributes.
//pthread_t             // Used to identify a thread.
//size_t                // Used for sizes of objects.
//ssize_t               // Used for a count of bytes or an error indication.
//suseconds_t           // Used for time in microseconds
//time_t                // Used for time in seconds.
//timer_t               // Used for timer ID returned by timer_create().
//useconds_t            // Used for time in microseconds.

#endif