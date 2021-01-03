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
    import SLEEPING

    section .text

TICKER      = $40c
INT_ADDR    = $f80011
CLEAR_INT   = ~$20
IN_SWAP     = $800  ; arbitrary and probably a bad choice

;struct context_t {
;    // Order matters here
;    uint32_t d[8];             // Data registers D1-D7
;    uint32_t a[7];             // Address registers A1-A6
;    uint32_t usp;              // User stack pointer
;    uint16_t sr;               // Status register (and really just CCR)
;    uint16_t _blank;
;    uint32_t pc;               // Program counter
;    struct context_t* next;    // Next process to run
;};

CP_D0     = 0
CP_D1     = CP_D0 + 4
CP_D2     = CP_D1 + 4
CP_D3     = CP_D2 + 4
CP_D4     = CP_D3 + 4
CP_D5     = CP_D4 + 4
CP_D6     = CP_D5 + 4
CP_D7     = CP_D6 + 4
CP_A0     = CP_D7 + 4
CP_A1     = CP_A0 + 4
CP_A2     = CP_A1 + 4
CP_A3     = CP_A2 + 4
CP_A4     = CP_A3 + 4
CP_A5     = CP_A4 + 4
CP_A6     = CP_A5 + 4
CP_A7     = CP_A6 + 4
CP_SP     = CP_A7      ; USP
CP_SR     = CP_SP + 4  ; Status register - really just 2 bytes
CP_PC     = CP_SR + 4  ; Program counter
CP_NEXT   = CP_PC + 4  ; Next process to run
CP_STATE  = CP_NEXT + 4

; context_swap gets a stack that looks like:
; 0xSP+4    | Program Counter Low  |
; 0xSP+2    | Program Counter High |
; 0xSP      | Status Register      |    <- we start here on entry

SP_SR    = 0
SP_PC    = 2

context_init::
    move.l #0, IN_SWAP
    rts

; Saves the current context and restores the next context in the list
context_swap::
    addi.l  #1, TICKER              ; Tick the next tick

    tas IN_SWAP                     ; Automic test and set
    bne context_swap_quick_done

    ; Set up
    move.l  a0, -(sp)               ; Free up a0 to do some work

    move.l  usp, a0                 ; Push user stack pointer
    move.l  a0, -(sp)

    move.l  current_process,a0      ; Get our current_process

    ; Save
    move.l  (sp)+, CP_SP(a0)
    move.l  (sp)+, CP_A0(a0)        ; SSP now back to where it started

    movem.l d0-d7/a0-a6, CP_D0(a0)
    movem.l a1-a6, CP_A1(a0)
    move.w  (sp), CP_SR(a0)         ; Save sr
    move.l  2(sp), CP_PC(a0)        ; Save pc

    ; Switch
next:
    move.l  CP_NEXT(a0), a0         ; Switch to next task
    move.l  a0, current_process

    ; Restore
    move.l  CP_PC(a0), 2(sp)        ; Restore pc
    move.w  CP_SR(a0), (sp)         ; Restore sr

    movem.l CP_D0(a0), d0-d7
    movem.l CP_A1(a0), a1-a6
    move.l  CP_SP(a0), a0           ; Restore USP
    move.l  a0, usp                 ; before we lose a0
    move.l  current_process, a0     ; Fix up a0 one last time
    move.l  CP_A0(a0), a0           ; to restore a0

    ; Return
context_swap_done:
    move.b  CLEAR_INT, INT_ADDR     ; Clear interrupt
    move.l #0,IN_SWAP               ; no longer in a swap
context_swap_quick_done:
    rte                             ; Return to next task
