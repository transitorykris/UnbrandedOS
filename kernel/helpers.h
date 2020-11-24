/*
Copyright 2020 Kris Foster
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

#ifndef HELPERS_H
#define HELPERS_H

typedef void (*fptr)();

#define SET_BYTE(v,l) \
{\
    uint8_t *p = (uint8_t *)l;\
    *p = v;\
}

#define SET_WORD(v,l) \
{\
    uint16_t *p = (uint16_t *)l;\
    *p = v;\
}

#define SET_LONG(v,l) \
{\
    uint32_t *p = (uint32_t *)l;\
    *p = v;\
}

#define SET_VECTOR(h,l) \
{\
    fptr *handler = (fptr *)l;\
    *handler = &h;\
}

#define GET_BYTE(l) *(unsigned char *)l
#define GET_WORD(l) *(unsigned int *)l
#define GET_LONG(l) *(unsigned long *)l

#define INC_BYTE(l) \
{\
    uint8_t *v = (uint8_t *)l;\
    (*v)++;\
}

#define INC_WORD(l) \
{\
    uint16_t *v = (uint16_t *)l;\
    (*v)++;\
}

#define INC_LONG(l) \
{\
    uint32_t *v = (uint32_t *)l;\
    (*v)++;\
}

#endif