/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|       firmware v1                 
 * ------------------------------------------------------------
 * Copyright (c)2020 Ross Bamford
 * See top-level LICENSE.md for licence information.
 *
 * This is the entry point for the POC "kernel".
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <machine.h>
#include <basicio.h>
#include "easy68k/easy68k.h"

#ifdef PERFORM_LINKAGE_CHECK
#include "linkcheck.h"
#endif

#define SYS_TICKS 0x40C
typedef unsigned int tick;

unsigned int get_ticks();

noreturn void kmain() {
  println("Kernel started");

#ifdef PERFORM_LINKAGE_CHECK
  checkLinkage();
#endif

  for(;;) {
    e68DisplayNumUnsigned(get_ticks(), 10);
    e68Println("");
  }
}

/*
  Returns the number of ticks since the system started
*/
tick get_ticks() {
  return *(unsigned int *)SYS_TICKS;
}
