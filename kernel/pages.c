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

#include <stdint.h>
#include <stdbool.h>

#include "spinlock.h"

#include "pages.h"

#define null    0x0;

spinlock_t pages_lock;  // hack until this is placed behind a trap

struct page_list *pages_start;
struct page_list *pages_last;


// !!! ALL OF THIS IS UNTESTED


// Initializes the page system
void init_pages() {
    pages_start = (struct page_list *)PAGES_START;
    pages_start->page = (page_t *)0x0;
    pages_start->next = pages_start + sizeof(struct page_list);
    pages_start->free = false;
    pages_last = pages_start;
}

// Allocates a page of memory to the caller
page_t *allocate_page() {
    // Don't do anything until it's our turn
    spinlock(&pages_lock);

    // the next struct of metadata is adjacent to the last byte
    // of the last allocation
    struct page_list *new_page = pages_last->next;
    pages_last = new_page;  // keep track of the last node

    // the location of the data is offset by the size of the header
    new_page->page = (page_t *)new_page + sizeof(struct page_list);

    // If we've run past the end of available, too bad
    if (new_page->page > PAGES_END) {
        return (void *)null;
    }

    // The next free memory location is after the last byte allocated
    // the 68010 will get upset if we get misaligned!
    new_page->next = (struct page_list *)new_page->page + PAGE_SIZE;

    // Not free anymore!
    new_page->free = false;

    spinunlock(&pages_lock);

    // Give the caller the start of their page
    return new_page->page;
}

// Frees the given page of memory
void free_page(page_t page) {
    // TO BE IMPLEMENTED
}