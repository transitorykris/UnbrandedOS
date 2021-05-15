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

TICKER      = $40c
INT_ADDR    = $f80011
CLEAR_INT   = ~$20
IN_SWAP     = $800  ; arbitrary and probably a bad 

; Defined in context.h
SLEEPING    = 0
RUNNING     = 1
EMBRYO      = 2
ZOMBIE      = 3

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