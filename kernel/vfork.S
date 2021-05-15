; Copyright 2021 Kris Foster
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.

    import current_process

    ;export vfork_handler
    export syscall_handler

    section .text

    include "context.inc"

; First step in handling a fork syscall, save the current process's context
syscall_handler:
    ori.w #$0700,sr

    move.l  a0, -(sp)               ; Free up a0 to do some work
    move.l  usp, a0                 ; Push user stack pointer
    move.l  a0, -(sp)

    move.l  current_process,a0      ; Get our current_process

    ; Save
    move.l  (sp)+, CP_SP(a0)
    move.l  (sp)+, CP_A0(a0)        ; SSP now back to where it started

    movem.l d0-d7, CP_D0(a0)
    movem.l a1-a6, CP_A1(a0)
    move.l  (sp), CP_SR(a0)        ; Save sr
    move.l  2(sp), CP_PC(a0)        ; Save pc

    ; Let's do the rest of the work in C
    jsr _vfork

    ; Restore our process, really just d0 should have changed
    move.l  current_process,a0      ; Get our current_process

    move.l  CP_PC(a0), 2(sp)        ; Restore pc
    move.w  CP_SR(a0), (sp)         ; Restore sr

    movem.l CP_D0(a0), d0-d7
    movem.l CP_A1(a0), a1-a6
    move.l  CP_SP(a0), a0           ; Restore USP
    move.l  a0, usp                 ; before we lose a0
    move.l  current_process, a0     ; Fix up a0 one last time
    move.l  CP_A0(a0), a0           ; to restore a0

    ; Return
    rte