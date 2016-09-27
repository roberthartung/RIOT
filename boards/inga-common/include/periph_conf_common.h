/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef INGA_COMMON_PERIPH_CONF_COMMON_H_
#define INGA_COMMON_PERIPH_CONF_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (8000000L)

//#define F_CPU                       8000000
//#define F_RC_OSCILLATOR             32768

/** @} */

/**
 * @brief   Timer configuration
 *
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only onces we can use here.
 *
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
//#define TIMER_0_ISRC        TIMER1_COMPC_vect
/** @} */

/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (1U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect
/** @} */


#define SPI_NUMOF           (1U)
#define SPI_0_EN            (1U)
#define SPI_1_EN            (1U)

//#define SPI_0_SCK_PORT       PORT_B
#define SPI_0_SCK_PORT_DIR   DDRB
#define SPI_0_SCK_PIN        7
//#define SPI_0_SCK_PIN_POS    DDB7

//#define SPI_0_MISO_PORT      PORT_B
#define SPI_0_MISO_PORT_DIR  DDRB
#define SPI_0_MISO_PIN       6
//#define SPI_0_MISO_PIN_POS   DDB6

//#define SPI_0_MOSI_PORT      PORT_B
#define SPI_0_MOSI_PORT_DIR  DDRB
#define SPI_0_MOSI_PIN       5
//#define SPI_0_MOSI_PIN_POS   DDB5

/** @{ */
#define MEGA_PRR PRR0
/** @} */

/**
 * @brief    Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
#define XTIMER_SHIFT                (0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_PERIPH_CONF_COMMON_H_ */
