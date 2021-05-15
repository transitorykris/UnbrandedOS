; Copyright 2020 Kris Foster
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
    import next_runnable

    section .text

    include "context.inc"

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

    movem.l d0-d7, CP_D0(a0)
    movem.l a1-a6, CP_A1(a0)
    move.w  (sp), CP_SR(a0)         ; Save sr
    move.l  2(sp), CP_PC(a0)        ; Save pc

    ; Switch to next runnable task
    jsr next_runnable

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
    move.l  #0, IN_SWAP             ; no longer in a swap
context_swap_quick_done:
    move.b  #~$20,INT_ADDR          ; Clear interrupt-in-service
    rte                             ; Return to next task
