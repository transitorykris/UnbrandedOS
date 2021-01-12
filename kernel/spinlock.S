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

    section .text

; This spinlock preserves all but the high-order bit
; All routines take a pointer to a byte

spinlock_init::
    move.l  (8,a7), a0      ; get location of lock
    andi.b  #0, (a0)        ; initialize as unlocked
    rts

spinlock::
    move.l  (8,a7), a0      ; get locatin of lock
    tas (a0)                ; test (a0)'s high-order bit, set if it's 0
    bne spinlock            ; if (a0) high-order bit was not 0, try again
    rts                     ; otherwise, it's now set, lock acquired

spinunlock::
    move.l (8,a7), a0       ; get location of lock
    eori.b  #128, (a0)      ; unlock the lock
    rts