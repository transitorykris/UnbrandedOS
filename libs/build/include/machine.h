/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|       firmware v1                 
 * ------------------------------------------------------------
 * Copyright (c)2019 Ross Bamford
 * See top-level LICENSE.md for licence information.
 *
 * C prototypes for system routines implemented in assembly.
 * ------------------------------------------------------------
 */
#ifndef _ROSCOM68K_MACHINE_H
#define _ROSCOM68K_MACHINE_H

#include <stdint.h>
#include <stdnoreturn.h>

#ifdef REVISION_0
// DEFINEs for MFP registers on Revision 0 board
#define MFP_GPDR      0xf80001
#define MFP_AER       0xf80021
#define MFP_DDR       0xf80011
#define MFP_IERA      0xf80031
#define MFP_IERB      0xf80009
#define MFP_IPRA      0xf80029
#define MFP_IPRB      0xf80019
#define MFP_ISRA      0xf80039
#define MFP_ISRB      0xf80005
#define MFP_IMRA      0xf80025
#define MFP_IMRB      0xf80015
#define MFP_VR        0xf80035
#define MFP_TACR      0xf8000D
#define MFP_TBCR      0xf8002D
#define MFP_TCDCR     0xf8001D
#define MFP_TADR      0xf8003D
#define MFP_TBDR      0xf80003
#define MFP_TCDR      0xf80023
#define MFP_TDDR      0xf80013
#define MFP_SCR       0xf80033
#define MFP_UCR       0xf8000B
#define MFP_RSR       0xf8002B
#define MFP_TSR       0xf8001B
#define MFP_UDR       0xf8003B
#else
// DEFINEs for MFP registers on "fixed" boards
#define MFP_GPDR      0xf80001
#define MFP_AER       0xf80003
#define MFP_DDR       0xf80005
#define MFP_IERA      0xf80007
#define MFP_IERB      0xf80009
#define MFP_IPRA      0xf8000B
#define MFP_IPRB      0xf8000D
#define MFP_ISRA      0xf8000F
#define MFP_ISRB      0xf80011
#define MFP_IMRA      0xf80013
#define MFP_IMRB      0xf80015
#define MFP_VR        0xf80017
#define MFP_TACR      0xf80019
#define MFP_TBCR      0xf8001B
#define MFP_TCDCR     0xf8001D
#define MFP_TADR      0xf8001F
#define MFP_TBDR      0xf80021
#define MFP_TCDR      0xf80023
#define MFP_TDDR      0xf80025
#define MFP_SCR       0xf80027
#define MFP_UCR       0xf80029
#define MFP_RSR       0xf8002B
#define MFP_TSR       0xf8002D
#define MFP_UDR       0xf8002F
#endif

/*
 * Absolute symbols defined in linker script
 */
extern uint32_t       _INITIAL_STACK;     // firmware stack top (mem top)
extern void           (*_WARM_BOOT)();      // firmware warm boot address

extern void           (*_MFP_VECTORS[16])();  // MFP interrupt vectors

extern uint32_t       _SDB_MAGIC;           // SDB magic number
extern uint32_t       _SDB_STATUS;          // SDB status code
extern volatile uint32_t _TIMER_100HZ;      // 100Hz timer counter
extern uint8_t        _EASY68K_ECHOON;      // Easy68k 'echo on' flag
extern uint8_t        _EASY68K_PROMPT;      // Easy68k 'prompt on' flag 
extern uint8_t        _EASY68K_SHOWLF;      // Easy68k 'LF display' flag
extern uint32_t       _SDB_MEM_SIZE;        // contiguous memory size

// NOTE: These are not generally callable from C
extern void           (*_EFP_PRINT)();        // ROM EFP vectors
extern void           (*_EFP_PRINTLN)();   
extern void           (*_EFP_PRINTCHAR)(); 
extern void           (*_EFP_HALT)();      
extern void           (*_EFP_SENDCHAR)();  
extern void           (*_EFP_RECVCHAR)();  
extern void           (*_EFP_CLRSCR)();    
extern void           (*_EFP_MOVEXY)();    
extern void           (*_EFP_SETCURSOR)(); 
extern void           (*_EFP_SETCURSOR)(); 
extern void           (*_EFP_RESRVD_444)();
extern void           (*_EFP_PROGLOADER)();

extern char           _FIRMWARE[];          // ROM firmware start address
extern uint32_t       _FIRMWARE_REV;        // rosco ROM firmware revision
extern char           _LOAD_ADDRESS[];      // firmware load address

/*
 * Early print null-terminated string.
 */
void mcPrint(char *str);

/*
 * Early print null-terminated string.
 */
void mcPrintln(char *str);

void mcSendchar(char c);
char mcReadchar();

/*
 * Busywait for a while. The actual time is wholly dependent
 * on CPU (i.e. clock) speed!
 * About 18 CPU cycles per "tick" (plus small overhead per call)
 *   ~2.25 usec per-tick with  8MHz CPU
 *   ~1.80 usec per tick with 10Mhz CPU
 */
void mcBusywait(uint32_t ticks);

/*
 * Delay for n 10ms ticks (using 100Hz timer interrupt)
 */
void mcDelaymsec10(uint32_t ticks10ms);

/*
 * Disable all interrupts (except NMI).
 */
void mcDisableInterrupts();

/*
 * Enable all interrupts.
 */
void mcEnableInterrupts();

/*
 * Disable interrupts and halt the machine. The only way to
 * recover from this is via wetware intervention.
 */
noreturn void mcHalt();

#endif

