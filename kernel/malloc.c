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

#include <ctype.h>
#include <stdbool.h>

#include "malloc.h"

#define null    0x0;

struct heap_list *heap_start;
struct heap_list *heap_last;

void init_heap() {
    heap_start = HEAP_START;
    heap_start->data = (void *)0x0;
    heap_start->next = heap_start + sizeof(struct heap_list);
    heap_start->free = false;   // never free
    heap_last = heap_start;
}

// Note: this isn't going to be reentrant
// will need to add some locking
void *malloc(size_t size) {
    // the next struct of metadata is adjacent to the last byte
    // of the last allocation
    struct heap_list *new_alloc = heap_last->next;
    heap_last = new_alloc;  // keep track of the last node

    // the location of the data is offset by the size of the header
    new_alloc->data = new_alloc + sizeof(struct heap_list);

    // If we've run past the end of available, too bad
    if (new_alloc->data > HEAP_END) {
        return (void *)null;
    }

    // The next free memory location is after the last byte allocated
    // the 68010 will get upset if we get misaligned!
    new_alloc->next = new_alloc->data + size + (size % 2);

    // Not free anymore!
    new_alloc->free = false;

    // We could calculate this, but for now just store it
    new_alloc->size = size;

    // Give the caller the start of their block of data
    return new_alloc->data;
}

void free(void *ptr) {
    // Incomplete implementation -- todo: merge adjacent free spaces
    struct heap_list *metadata = ptr - sizeof(struct heap_list);
    metadata->free = true;
}

void *calloc(size_t nmemb, size_t size) {
    // To be implemented
}

void *realloc(void *ptr, size_t size) {
    // To be implemented
}

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    // To be implemented
}