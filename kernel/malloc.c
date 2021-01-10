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

#include "malloc.h"

struct heap_list *heap_start;
struct heap_list *heap_last;

void init_heap() {
    heap_start = HEAP_START;
    heap_start->location = (void *)0x0;
    heap_start->next = heap_start + sizeof(struct heap_list);
    heap_last = heap_start;
}

// Note: this isn't going to be reentrant
// will need to add some locking
void *malloc(size_t size) {
    struct heap_list *start_next = heap_last;
    
    start_next->location = heap_last + sizeof(struct heap_list);
    if (start_next->location > HEAP_END) {
        return (void *)0x0;
    }
    
    start_next->next = start_next->location + size;
    
    return start_next->location;
}

void free(void *ptr) {
    // To be implemented
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