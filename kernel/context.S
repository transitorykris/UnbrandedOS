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
    import ssp

    section .text

TICKER      = $40c
INT_ADDR    = $f80011
CLEAR_INT   = ~$20

; Order matters! Append only
;typedef struct {
;    uint32_t a[8];      // Address registers
;    uint32_t d[8];      // Data registers
;    uint32_t usp;       // User stack pointer
;    uint16_t sr;        // Status register (and really just CCR)
;    uint32_t pc;        // Program counter
;} context_t;

C_DR_OFFSET     = 0
C_AR_OFFSET     = 32
C_USP_OFFSET    = 60
C_SR_OFFSET     = 64
C_PC_OFFSET     = 66

; - stack contains some goodies about the current task
; 0xSP-12 | USP                  |
; 0xSP-8  | Scratch A0           |
; 0xSP-4  | Scratch D0           |
; 0xSP    | Status Register      | <- sp
; 0xSP+2  | Program Counter High |
; 0xSP+4  | Program Counter Low  |

SP_SR_OFFSET = 0
SP_PC_OFFSET = 2
SP_SCRATCH_OFFSET = -8   ; D0 and A0
SP_USP_OFFSET = -12

; Context swap attempt number 283
; Notes
; - the context_swap's stack pointer is in sp
; - the user's stack pointer is in usp
; - we want to save SR, PC, USP, and D0-D7 and A0-A7
context_swap::
    addi.l #1,TICKER            ; Tick the next tick

    movem.l d0/a0,SP_SCRATCH_OFFSET(sp) ; Create scratch register (offset?)
    
    move.l usp,a0               ; Save user stack pointer to our stack
    move.l a0,SP_USP_OFFSET(sp)

    lea current_process,a0      ; Get our current_process

    ; Save
    movem.l d1-d7/a1-a6,C_DR_OFFSET(a0) ; Save bulk registers, a7 is our SP
    
    move.l SP_USP_OFFSET(sp),C_USP_OFFSET(a0)    ; Save USP

    move.l (sp),C_SR_OFFSET(a0) ; Save SR
    move.l SP_PC_OFFSET(sp),C_PC_OFFSET(a0) ; Save PC

    ; Next context


    ; Restore
    move.l C_SR_OFFSET(a0),SP_SR_OFFSET(sp) ; Save SR
    move.l C_PC_OFFSET(a0),SP_PC_OFFSET(sp) ; Save PC

    move.l C_USP_OFFSET(a0),a1          ; Restore USP
    move.l a1,usp

    movem.l C_DR_OFFSET(a0),d1-d7/a1-a6, ; Restre bulk registers

    movem.l SP_SCRATCH_OFFSET(sp),d0/a0         ; Restore scratch registers

    ; We're done!
    move.b CLEAR_INT,INT_ADDR   ; Clear interrupt
    rte                         ; Return to next task
