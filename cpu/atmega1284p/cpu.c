/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega1284p
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"
#include <avr/wdt.h>

#if (__AVR_LIBC_VERSION__ >= 10700UL)
/* The proper way to set the signature is */
#include <avr/signature.h>
#else

/* signature API not available before avr-lib-1.7.0. Do it manually.*/
typedef struct {
  const unsigned char B2;
  const unsigned char B1;
  const unsigned char B0;
} __signature_t;
#define SIGNATURE __signature_t __signature __attribute__((section (".signature")))
SIGNATURE = {
  .B2 = 0x05, //SIGNATURE_2, //ATMEGA1284p
  .B1 = 0x97, //SIGNATURE_1, //128KB flash
  .B0 = 0x1E, //SIGNATURE_0, //Atmel
};
#endif /* (__AVR_LIBC_VERSION__ >= 10700UL) */

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
  wdt_disable();

  periph_init();

  //
  // /// clock init
  // cli();
  // /* Clock from crystal on TOSC0-1 */
  // ASSR = _BV(AS2);
  // /* Set counter to zero */
  // TCNT0 = 0;
  // /*
  //  * Set comparison register:
  //  * Crystal freq. is 32768,
  //  * pre-scale factor is 8, we want CLOCK_CONF_SECOND ticks / sec:
  //  * 32768 = 8 * CLOCK_CONF_SECOND * OCR2A, less 1 for CTC mode
  //  */
  // OCR2A = 8000000UL/1024UL/1024;
  // /*
  //  * Set timer control register:
  //  *  - prescale: 8 (CS21)
  //  *  - counter reset via comparison register (WGM21)
  //  */
  // TCCR2A = _BV(WGM21);
  // TCCR2B = _BV(CS21);
  // /* Clear interrupt flag register */
  // TIFR2 = TIFR2;
  // /*
  //  * Raise interrupt when value in OCR2 is reached. Note that the
  //  * counter value in TCNT2 is cleared automatically.
  //  */
  // TIMSK2 = _BV(OCIE2A);
  //
  // sei();
}
