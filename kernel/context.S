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
    import context_switches

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

CP_D0     = 0
CP_D1     = 4
CP_D2     = 8
CP_D3     = 12
CP_D4     = 16
CP_D5     = 20
CP_D6     = 24
CP_D7     = 28
CP_A0     = 32
CP_A1     = 36
CP_A2     = 40
CP_A3     = 44
CP_A4     = 48
CP_A5     = 52
CP_A6     = 56
CP_A7     = 60  ; USP
CP_SR     = 64  ; Status register
CP_PC     = 66  ; Program counter
CP_NEXT   = 70  ; Next process to run

; Stack contains some goodies about the current task
; We also save a0 and d0 so we have space to work in
; USP is saved because we can't move directly from usp to our struct
; 0xSP      | USP                  |    <- sp will eventually be here
; 0xSP-4    | Scratch A0           |
; 0xSP-8    | Scratch D0           |
; 0xSP-12   | Status Register      |    <- we start here on entry
; 0xSP-14   | Program Counter High |
; 0xSP-18   | Program Counter Low  |

SP_SR    = 0
SP_PC    = 2

; Saves the current context and restores the next context in the list
context_swap::
    addi.l #1,TICKER                ; Tick the next tick

    ; Check if we interrupted while running in supervisor
    btst #5,(sp)
    bne done           ; If yes, bail out
    ;addi.l #1,context_switches
    ; Todo - I believe if this is true we save a7 rather than usp

    ; Set up
    move.l d0,-(sp)                 ; Free up scratch registers
    move.l a0,-(sp)

    move.l usp,a0                   ; Push user stack pointer
    move.l a0,-(sp)

    move.l current_process,a0       ; Get our current_process

    ; Save
    move.l (sp)+,CP_A7(a0)
    move.l (sp)+,CP_A0(a0)
    move.l (sp)+,CP_D0(a0)          ; SP now back to where we started

    move.l d1,CP_D1(a0)
    move.l d2,CP_D2(a0)
    move.l d3,CP_D3(a0)
    move.l d4,CP_D4(a0)
    move.l d5,CP_D5(a0)
    move.l d6,CP_D6(a0)
    move.l d7,CP_D7(a0)
    move.l a1,CP_A1(a0)
    move.l a2,CP_A2(a0)
    move.l a3,CP_A3(a0)
    move.l a4,CP_A4(a0)
    move.l a5,CP_A5(a0)
    move.l a6,CP_A6(a0)
    move.w (sp),CP_SR(a0)       ; Save sr
    move.l 2(sp),CP_PC(a0)       ; Save pc

    ; Switch
    move.l CP_NEXT(a0),a0            ; Switch to next task
    move.l a0,current_process

    ; Restore
    move.l CP_PC(a0),2(sp)       ; Restore pc
    move.w CP_SR(a0),(sp)       ; Restore sr

    move.l CP_D1(a0),d1
    move.l CP_D2(a0),d2
    move.l CP_D3(a0),d3
    move.l CP_D4(a0),d4
    move.l CP_D5(a0),d5
    move.l CP_D6(a0),d6
    move.l CP_D7(a0),d7
    move.l CP_A1(a0),a1
    move.l CP_A2(a0),a2
    move.l CP_A3(a0),a3
    move.l CP_A4(a0),a4
    move.l CP_A5(a0),a5
    move.l CP_A6(a0),a6

    move.l CP_D0(a0),d0              ; Restore d0

    move.l CP_A7(a0),a0              ; Restore USP
    move.l a0,usp

    move.l current_process,a0        ; Fix up a0 one last time
    move.l CP_A0(a0),a0              ; to restore a0

    ; Return
done:
    move.b CLEAR_INT,INT_ADDR       ; Clear interrupt
    rte                             ; Return to next task
