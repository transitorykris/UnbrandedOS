; Copyright 2020 Kris Foster
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

    section .text

TICKER      = $40c
INT_ADDR    = $f80011
CLEAR_INT   = ~$20

; Order matters! Append only
;typedef struct {
;    uint32_t d[7];      // Data registers D1-D7
;    uint32_t a[6];      // Address registers A1-A6, a7 will be stored in usp
;    uint32_t d0;        // Saved separately from ^
;    uint32_t a0;        // Ditto
;    uint32_t usp;       // User stack pointer
;    uint16_t sr;        // Status register (and really just CCR)
;    uint32_t pc;        // Program counter
;    struct context_t* next;     // Next process to run
;} context_t;

C_DR     = 0     ; Data registers
C_AR     = 28    ; Address registers
C_D0     = 52
C_A0     = 56
C_USP    = 60    ; User stack pointer
C_SR     = 64    ; Status register
C_PC     = 66    ; Program counter
C_NEXT   = 70    ; Next process to run

; Stack contains some goodies about the current task
; We also save a0 and d0 so we have space to work in
; USP is saved because we can't move directly from usp to our struct
; 0xSP      | USP                  |    <- sp will eventually be here
; 0xSP-4    | Scratch A0           |
; 0xSP-8    | Scratch D0           |
; 0xSP-12   | Status Register      |
; 0xSP-14   | Program Counter High |
; 0xSP-16   | Program Counter Low  |

SP_PC    = 14
SP_SR    = 12
SP_D0    = 8
SP_A0    = 4
SP_USP   = 0

; Saves the current context and restores the next context in the list
context_swap::
    addi.l #1,TICKER                ; Tick the next tick
    
    move.l d0,-(sp)                 ; Free up scratch registers
    move.l a0,-(sp)

    move.l usp,a0                   ; Push user stack pointer
    move.l a0,-(sp)

    move.l (current_process),a0     ; Get our current_process

    movem.l d1-d7/a1-a6,C_DR(a0)   ; Save bulk registers
    move.l SP_D0(sp),C_D0(a0)       ; Save d0
    move.l SP_A0(sp),C_A0(a0)       ; Save a0
    move.l SP_USP(sp),C_USP(a0)     ; Save usp
    move.w SP_SR(sp),C_SR(a0)       ; Save sr
    move.l SP_PC(sp),C_PC(a0)       ; Save pc

    ; XXX this isn't doing what I think it should do!!
    move.l C_NEXT(a0),a0            ; Switch to next task
    move.l a0,(current_process)

    move.w C_SR(a0),SP_SR(sp)       ; Restore sr
    move.l C_PC(a0),SP_PC(sp)       ; Restore pc
    movem.l C_DR(a0),d1-d7/a1-a6    ; Restore bulk registers
    move.l C_D0(a0),SP_D0(sp)       ; Prepare to restore d0
    move.l C_A0(a0),SP_A0(sp)       ; Prepare to restore a0
    move.l C_USP(a0),a0
    move.l a0,usp
    move.l SP_D0(sp),d0             ; Restore d0
    move.l SP_A0(sp),a0             ; Restore a0

    add.l #SP_SR,sp                 ; Reset our stack pointer

    move.b CLEAR_INT,INT_ADDR       ; Clear interrupt
    rte                             ; Return to next task
