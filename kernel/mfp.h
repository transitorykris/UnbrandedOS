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

#ifndef MFP_H
#define MFP_H

#define MFP_TIMER_C     0x114
#define MFP_ISRB        0xf80011

#define TRAP_0_VECTOR   0x80
#define TRAP_1_VECTOR   0x84
#define TRAP_2_VECTOR   0x88
#define TRAP_3_VECTOR   0x8C
#define TRAP_4_VECTOR   0x90
#define TRAP_5_VECTOR   0x94
#define TRAP_6_VECTOR   0x98
#define TRAP_7_VECTOR   0x9C
#define TRAP_8_VECTOR   0xA0
#define TRAP_9_VECTOR   0xA4
#define TRAP_10_VECTOR  0xA8
#define TRAP_11_VECTOR  0xAC
#define TRAP_12_VECTOR  0xB0
#define TRAP_13_VECTOR  0xB4
#define TRAP_14_VECTOR  0xB8
#define TRAP_15_VECTOR  0xB8

#define MFP_VECBASE     0x40    // Start of the vector table in memory

// Locations of each vector in the vector table
#define VEC_BUS_ERROR_HANDLER           0x02 // Bus Error
#define VEC_ADDRESS_ERROR_HANDLER       0x03 // Address Error
#define VEC_ILLEGAL_INSTRUCTION_HANDLER 0x04 // Illegal Instruction
#define VEC_DIV_BY_ZERO_HANDLER         0x05 // Divide by Zero
#define VEC_CHK_INSTRUCTION_HANDLER     0x06 // CHK Instruction
#define VEC_TRAPV_HANDLER               0x07 // TRAPV Instruction
#define VEC_PRIV_VIOLATION_HANDLER      0x08 // Privilege Violation
#define VEC_TRACE_HANDLER               0x09 // Trace
#define VEC_LINE_1010_HANDLER           0x0A // Line 1010 Emulator
#define VEC_LINE_1111_HANDLER           0x0B // Line 1111 Emulator
#define VEC_RESERVED_HANDLER_0          0x0C // Reserved
#define VEC_RESERVED_HANDLER_1          0x0D // Reserved
#define VEC_FORMAT_ERROR_HANDLER        0x0E // Format error (MC68010 Only)
#define VEC_GENERIC_HANDLER_13          0x0F // Uninitialized Vector
#define VEC_RESERVED_HANDLER_2          0x10 // Reserved
#define VEC_RESERVED_HANDLER_3          0x11 // Reserved
#define VEC_RESERVED_HANDLER_4          0x12 // Reserved
#define VEC_RESERVED_HANDLER_5          0x13 // Reserved
#define VEC_RESERVED_HANDLER_6          0x14 // Reserved
#define VEC_RESERVED_HANDLER_7          0x15 // Reserved
#define VEC_RESERVED_HANDLER_8          0x16 // Reserved
#define VEC_RESERVED_HANDLER_9          0x17 // Reserved
#define VEC_GENERIC_HANDLER_14          0x18 // Spurious Interrupt
#define VEC_INTERRUPT_HANDLER_0         0x19 // Level 1 Autovector
#define VEC_INTERRUPT_HANDLER_1         0x1A // Level 2 Autovector
#define VEC_INTERRUPT_HANDLER_2         0x1B // Level 3 Autovector
#define VEC_INTERRUPT_HANDLER_3         0x1C // Level 4 Autovector
#define VEC_INTERRUPT_HANDLER_4         0x1D // Level 5 Autovector
#define VEC_INTERRUPT_HANDLER_5         0x1E // Level 6 Autovector
#define VEC_INTERRUPT_HANDLER_6         0x1F // Level 7 Autovector
#define VEC_TRAP_0_HANDLER              0x20 // TRAP Handlers (unused)
#define VEC_TRAP_1_HANDLER              0x21 // TRAP Handlers (unused)
#define VEC_TRAP_2_HANDLER              0x22 // TRAP Handlers (unused)
#define VEC_TRAP_3_HANDLER              0x23 // TRAP Handlers (unused)
#define VEC_TRAP_4_HANDLER              0x24 // TRAP Handlers (unused)
#define VEC_TRAP_5_HANDLER              0x25 // TRAP Handlers (unused)
#define VEC_TRAP_6_HANDLER              0x26 // TRAP Handlers (unused)
#define VEC_TRAP_7_HANDLER              0x27 // TRAP Handlers (unused)
#define VEC_TRAP_8_HANDLER              0x28 // TRAP Handlers (unused)
#define VEC_TRAP_9_HANDLER              0x29 // TRAP Handlers (unused)
#define VEC_TRAP_10_HANDLER             0x2A // TRAP Handlers (unused)
#define VEC_TRAP_11_HANDLER             0x2B // TRAP Handlers (unused)
#define VEC_TRAP_12_HANDLER             0x2C // TRAP Handlers (unused)
#define VEC_TRAP_13_HANDLER             0x2D // TRAP Handlers (unused)
#define VEC_TRAP_14_HANDLER             0x2E // TRAP#14 handler
#define VEC_TRAP_15_HANDLER             0x2F // TRAP#15 handler (replaced later)
#define VEC_RESERVED_HANDLER_10         0x30 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_11         0x31 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_12         0x32 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_13         0x33 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_14         0x34 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_15         0x35 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_16         0x36 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_17         0x37 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_18         0x38 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_19         0x39 // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_20         0x3A // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_21         0x3B // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_22         0x3C // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_23         0x3D // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_24         0x3E // Remaining Reserved vectors
#define VEC_RESERVED_HANDLER_25         0x3F // Remaining Reserved vectors
#define VEC_UNMAPPED_GPIO_HANDLER_0     0x40 // MFP GPIO #0 (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_1     0x41 // MFP GPIO #1 (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_2     0x42 // MFP GPIO #2 (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_3     0x43 // MFP GPIO #3 (Not used)
#define VEC_TIMER_D_HANDLER             0x44 // MFP Timer D (Interrupt not used)
#define VEC_TICK_HANDLER                0x45 // MFP Timer C (System tick)
#define VEC_UNMAPPED_GPIO_HANDLER_4     0x46 // MFP GPIO #4 (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_5     0x47 // MFP GPIO #5 (Not used)
#define VEC_TIMER_B_HANDLER             0x48 // MFP Timer B (Not used)
#define VEC_TRANSMITTER_ERROR_HANDLER   0x49 // Transmitter error (Not used)
#define VEC_TRANSMITTER_EMPTY_HANDLER   0x4A // Transmitter empty (Replaced later)
#define VEC_RECV_ERROR_HANDLER          0x4B // Receiver error (Replaced later)
#define VEC_RECV_BUFFER_FULL_HANDLER    0x4C // Receiver buffer full (Replaced later)
#define VEC_TIMER_A_HANDLER             0x4D // Timer A (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_6     0x4E // MFP GPIO #6 (Not used)
#define VEC_UNMAPPED_GPIO_HANDLER_7     0x4F // MFP GPIO #6 (Not used)

// Additional user handlers exist 0x50 - 0xFF

#endif