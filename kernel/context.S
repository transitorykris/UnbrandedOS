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
;    uint32_t d[7];      // Data registers D1-D7
;    uint32_t a[6];      // Address registers A1-A6, a7 will be stored in usp
;    uint32_t d0;        // Saved separately from ^
;    uint32_t a0;        // Ditto
;    uint32_t usp;       // User stack pointer
;    uint16_t sr;        // Status register (and really just CCR)
;    uint32_t pc;        // Program counter
;    struct context_t* next;     // Next process to run
;} context_t;

C_DR_OFFSET     = 0     ; Data registers
C_AR_OFFSET     = 28    ; Address registers
C_D0_OFFSET     = 52
C_A0_OFFSET     = 56
C_USP_OFFSET    = 60    ; User stack pointer
C_SR_OFFSET     = 64    ; Status register
C_PC_OFFSET     = 66    ; Program counter
C_NEXT_OFFSET   = 70    ; Next process to run

; - stack contains some goodies about the current task
; 0xSP      | USP                  |    <- sp will eventually be here
; 0xSP-4    | Scratch A0           |
; 0xSP-8    | Scratch D0           |
; 0xSP-12   | Status Register      |
; 0xSP-14   | Program Counter High |
; 0xSP-16   | Program Counter Low  |

SP_PC_OFFSET    = 14
SP_SR_OFFSET    = 12
SP_D0_OFFSET    = 8
SP_A0_OFFSET    = 4
SP_USP_OFFSET   = 0

; Context swap attempt number 283
; Notes
; - the context_swap's stack pointer is in sp
; - the user's stack pointer is in usp
; - we want to save SR, PC, USP, and D0-D7 and A0-A7
context_swap::
    addi.l #1,TICKER            ; Tick the next tick

    ; Monkey with the stack pointer to make offsets easier?

    ; Free up scratch registers
    ;movem.l d0/a0,SP_SCRATCH_OFFSET(sp) ; Create scratch register (offset?)
    move.l d0,-(sp)
    move.l a0,-(sp)

    ; Save user stack pointer to our stack using the newly freed register
    move.l usp,a0               ; Save user stack pointer to our stack
    move.l a0,-(sp)

    ; sp should now represent the offsets we defined above

    ; Load our current_process struct
    lea current_process,a0      ; Get our current_process

    ; Save
    movem.l d1-d7/a1-a6,(C_DR_OFFSET,a0) ; Save bulk registers, a7 is our SP
    move.l (SP_D0_OFFSET,sp),(C_D0_OFFSET,a0)
    move.l (SP_A0_OFFSET,sp),(C_A0_OFFSET,a0)
    move.l (SP_USP_OFFSET,sp),(C_USP_OFFSET,a0)    ; Save USP

    ; Save the current SR and PC
    move.w (SP_SR_OFFSET,sp),(C_SR_OFFSET,a0) ; Save SR
    move.l (SP_PC_OFFSET,sp),(C_PC_OFFSET,a0) ; Save PC

    ; Next context - XXX make sure this is right
    ; WTFFFFffff
    ;lea (C_NEXT_OFFSET,a0),a0

    ; Restore the SR and PC
    move.w (C_SR_OFFSET,a0),(SP_SR_OFFSET,sp) ; Save SR
    move.l (C_PC_OFFSET,a0),(SP_PC_OFFSET,sp) ; Save PC

    ; Restore D0-D7, A0-A6, and USP
    movem.l (C_DR_OFFSET,a0),d1-d7/a1-a6 ; Restore bulk registers

    move.l (C_D0_OFFSET,a0),(SP_D0_OFFSET,sp)
    move.l (C_A0_OFFSET,a0),(SP_A0_OFFSET,sp)
    
    move.l (C_USP_OFFSET,a0),(SP_USP_OFFSET,sp)
    move.l (SP_USP_OFFSET,sp),a0
    move.l a0,usp

    move.l (SP_D0_OFFSET,sp),d0
    move.l (SP_A0_OFFSET,sp),a0


    ; Move our stack pointer back to where it started for RTE to work!
    add.l #SP_SR_OFFSET,sp

    ; We're done!
    move.b CLEAR_INT,INT_ADDR   ; Clear interrupt
    rte                         ; Return to next task
