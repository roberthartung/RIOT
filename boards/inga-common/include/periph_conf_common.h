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

/*
#define TIMER_1             MEGA_TIMER2
#define TIMER_1_MASK        &TIMSK2
#define TIMER_1_FLAG        &TIFR2
#define TIMER_1_ISRA        TIMER2_COMPA_vect
#define TIMER_1_ISRB        TIMER2_COMPB_vect
*/
// #define TIMER_0_ISRC        TIMER1_COMPOVF
// #define TIMER_0_ISRD        TIMER1_COMPCAPT
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

/**
 * The INGA has exactly 1 I2C interface
 * @{
*/
#define I2C_NUMOF           (1U)
#define I2C_0_EN            (1)
#define I2C_0_SCL           GPIO_PIN(PORT_C, 0)
#define I2C_0_SDA           GPIO_PIN(PORT_C, 1)
/** @} */

#define ADXL345_PARAM_ADDR  ADXL345_ADDR_53 // (0xA6>>1)
#define ADXL345_PARAM_I2C   (0) // Device 0

#define ADXL345_PARAMS              { .offset = ADXL345_PARAM_OFFSET,    \
                                      .range  = ADXL345_RANGE_2G,     \
                                      .rate   = ADXL345_RATE_100HZ,      \
                                      .full_res = ADXL345_PARAM_FULL_RES }

#define L3G4200D_PARAM_ADDR (0x69) // 0xD2>>1

#define SPI_NUMOF           (1U)
/*
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
*/

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_PERIPH_CONF_COMMON_H_ */
