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

#ifndef FS_H
#define FS_H

#define MAX_FILES       64

#define FILE        0
#define DIRECTORY   1

typedef struct {
    // XXX very temporary hack
    void (*start)();             // Location on device
} inode_t;

typedef struct {
    char        *name;          // Human readable name
    uint8_t     type;           // Type of this file
    inode_t     inode;          // Points to the file's first inode
} file_t;

typedef struct {
    file_t files[MAX_FILES];    // All files in this directory
} directory_t;

typedef struct {
    directory_t* root;          // Root of the filesystem hierarchy
} filesystem_t;

void init_filesystem();
void create_file(char *name, uint8_t type, void (*inode_start)());
void delete_file(char *name);
void list_directory();
uint8_t exec(char *name);

// We'll create one global filesystem for now
filesystem_t fs;
directory_t root;

#endif