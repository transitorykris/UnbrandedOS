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

; This will eventually become our context swap routine
; For now just learning how to call assembly routines
context_swap::
    addi.l #1,$40C              ; Increment tick counter
    
    move.l a7,ssp              ; Save the location of the stack pointer
    move.l (current_process),a7   ; Replace it with current_process
    movem.l d0-d7/a0-a6,-(a7)   ; Save registers to current_process
    move usp,a0                 ; Save user stack pointer
    move a0,-(a7)

    ; Swap here....

    move (a7)+,a0               ; Restore usp
    move a0,usp
    movem.l (a7)+,d0-d7/a0-a6   ; Restore registers
    move.l ssp,a7                 ; Restore supervisor stack pointer
    
    move.b #~$20,$f80011        ; Clear interrupt

    rte                         ; Return to next process!

context_swap_o::
    addi.l #1,$40C
    movem.l d0-d7/a0-a6,-(a7)   ; Save the previous registers
    move usp,a0                 ; Save the previous user stack pointer
    move a0,-(a7)
    
    ; Save PC, SR, registers, USP to struct
    ;move.l (a7)+,(current_process)    ; usp
    ;move.l (a7)+,current_process    ; a6
    ;move.l (a7)+,current_process    ; a5
    ;move.l (a7)+,current_process    ; a4
    ;move.l (a7)+,current_process    ; a3
    ;move.l (a7)+,current_process    ; a2
    ;move.l (a7)+,current_process    ; a1
    ;move.l (a7)+,current_process    ; a0
    ;move.l (a7)+,current_process    ; d7
    ;move.l (a7)+,current_process    ; d6
    ;move.l (a7)+,current_process    ; d5
    ;move.l (a7)+,current_process    ; d4
    ;move.l (a7)+,current_process    ; d3
    ;move.l (a7)+,current_process    ; d2
    ;move.l (a7)+,current_process    ; d1
    ;move.l (a7)+,current_process    ; d0
    ;move.w (a7)+,current_process    ; sr
    ;move.w (a7)+,current_process    ; pc high
    ;move.l (a7)+,current_process    ; pc low

    ; Load new PC, SR, registers, and USP
    ; Save PC, SR, registers, USP to struct
    ;move.l -(a7),current_process    ; usp
    ;move.l -(a7),current_process    ; a6
    ;move.l -(a7),current_process    ; a5
    ;move.l -(a7),current_process    ; a4
    ;move.l -(a7),current_process    ; a3
    ;move.l -(a7),current_process    ; a2
    ;move.l -(a7),current_process    ; a1
    ;move.l -(a7),current_process    ; a0
    ;move.l -(a7),current_process    ; d7
    ;move.l -(a7),current_process    ; d6
    ;move.l -(a7),current_process    ; d5
    ;move.l -(a7),current_process    ; d4
    ;move.l -(a7),current_process    ; d3
    ;move.l -(a7),current_process    ; d2
    ;move.l -(a7),current_process    ; d1
    ;move.l -(a7),current_process    ; d0
    ;move.w -(a7),current_process    ; sr
    ;move.w -(a7),current_process    ; pc high
    ;move.l -(a7),current_process    ; pc low

    ; TODO move these back from the new context struct
    move (a7)+,a0                 ; Save the previous user stack pointer
    move a0,usp
    movem.l (a7)+,d0-d7/a0-a6   ; Restore the new registers
    move.b #~$20,$f80011
    rte

;context_swap::
;    addi.l #1,$40C              ; Context swap counter ;)
;    movem.l d0-d7/a1-a6,-(sp)   ; Save the previous registers
;    move usp,a0                 ; Save the previous user stack pointer
;    move.l a0,-(sp)
;    
;    ; Save PC, SR, registers, USP to struct
;
;    ; TODO replace USP, PC, and SR on the stack
;    move.l (sp)+,a0
;    move a0,usp                 ; Restore the new user stack pointer
;
;    ; TODO move these back from the new context struct
;    movem.l (sp)+,d0-d7/a0-a7   ; Restore the new registers
;    rts
