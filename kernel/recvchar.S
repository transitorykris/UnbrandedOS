; Copyright 2021 Kris Foster
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

    include "../shared/equates.S"

; Falls through returning NULL if no characters are waiting
RECVCHAR_MFP_NONBLOCKING::
    bclr.b  #7,MFP_GPDR         ; Lower RTS
.BEGIN
    move.b  MFP_RSR,D0          ; Get RSR
    btst    #7,D0               ; Is buffer_full bit set?
    bne.s   .GOTCHR             ; Yes - Go to receive character

    btst    #6,D0               ; Else, do we have an overrun error?
    bne.s   .GOTERR             ; .. Yes - handle that
    bra.s   .NOCHR              ; No - we're out

.GOTERR
    move.b  MFP_UDR,D0          ; Empty buffer
    bchg.b  #1,MFP_GPDR         ; And toggle I1
    bra.s   .BEGIN              ; And continue testing...
    
.GOTCHR
    move.b  MFP_UDR,D0          ; Get the data
    rts

.NOCHR
    move.b #0,D0                ; Send a NULL otherwise
    rts