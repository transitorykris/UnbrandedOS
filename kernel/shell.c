/*
Copyright 2021 Kris Foster
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <machine.h>
#include <basicio.h>

#include "system.h"

#include "shell.h"

int readline_nonblocking(char *buf, int buf_size);

void shell() {
    println("Starting shell");
    for(;;) {
        printuint(get_ticks());
        print(" # ");
        // readline is currently a blocking call...
        for (;;) {
            if (readline_nonblocking(buf, BUFFER_LEN) > 0) {
                println(buf);
                break;
            }
        }
    }
}

static char backspace[4] = { 0x08, 0x20, 0x08, 0x00 };
static char sendbuf[2] = { 0x00, 0x00 };

int readline_nonblocking(char *buf, int buf_size) {
  register char c;
  register uint8_t i = 0;

  while (i < buf_size - 1) {
    c = buf[i] = mcReadchar();

    switch (c) {
    case 0x00:  // No character read
        continue;
    case 0x08:
    case 0x7F:  /* DEL */
      if (i > 0) {
        buf[i-1] = 0;
        i = i - 1;
        mcPrint(backspace);
      }
      break;
    case 0x0A:
      // throw this away...
      break;
    case 0x0D:
      // return
      buf[i] = 0;
      mcPrintln("");
      return i;
    default:
      buf[i++] = c;
      sendbuf[0] = c;
      mcPrint(sendbuf);
    }
  }

  buf[buf_size-1] = 0;
  return buf_size;
}

/*
.RECVCHAR
    move.l  A1,-(A7)
    move.l  EFP_RECVCHAR,A1
    jsr     (A1)
    move.l  (A7)+,A1
    rte

RECVCHAR_MFP:
    bclr.b  #7,MFP_GPDR           ; Lower RTS
.BEGIN
    move.b  MFP_RSR,D0            ; Get RSR
    btst    #7,D0                 ; Is buffer_full bit set?
    bne.s   .GOTCHR               ; Yes - Go to receive character

    btst    #6,D0                 ; Else, do we have an overrun error?
    bne.s   .GOTERR               ; .. Yes - handle that
    bra.s   .BEGIN                ; .. No - Just loop

.GOTERR
    move.b  MFP_UDR,D0            ; Empty buffer
    bchg.b  #1,MFP_GPDR           ; And toggle I1
    bra.s   .BEGIN                ; And continue testing...
    
.GOTCHR
    move.b  MFP_UDR,D0            ; Get the data
    rts
*/