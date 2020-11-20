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

//extern void easy68k_example();

//static volatile uint8_t * const mfp_gpdr = (uint8_t * const)MFP_GPDR;
//static char readline_buf[1024];

noreturn void kmain() {
  println("Kernel started");

#ifdef PERFORM_LINKAGE_CHECK
  checkLinkage();
#endif

  const unsigned int *ticks = (const unsigned int *)0x40C;
  for(;;) {
    e68DisplayNumUnsigned(*ticks, 10);
    e68Println("");
  }

  //easy68k_example();

  //while (true) {
  //  *(mfp_gpdr) ^= 2;
  //
  //  if (readline(readline_buf, 1024) > 0) {
  //    println(readline_buf);
  //  }
  //}
}

