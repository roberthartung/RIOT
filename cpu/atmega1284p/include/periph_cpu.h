/*
 * Copyright (C) 2016 Technische Universität Braunschweig, IBR
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atmega1284p
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Robert Hartung <hartung@ibr.cs.tu-bs.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "periph_cpu_common.h"

#define PM_NUM_MODES (6)

enum pm_sleep_modes {
    PM_SLEEPMODE_PWR_DOWN    = 0,
    PM_SLEEPMODE_STANDBY     = 1,
    PM_SLEEPMODE_PWR_SAVE    = 2,
    PM_SLEEPMODE_EXT_STANDBY = 3,
    PM_SLEEPMODE_ADC         = 4,
    PM_SLEEPMODE_IDLE        = 5,
};

/// The level where the SPI cannot operate anymore
#define PM_SLEEPMODE_INVALID_TWI     PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_SPI     PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_UART0   PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_UART1   PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_TIMER0  PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_TIMER1  PM_SLEEPMODE_ADC
#define PM_SLEEPMODE_INVALID_TIMER2  PM_SLEEPMODE_STANDBY
#define PM_SLEEPMODE_INVALID_TIMER3  PM_SLEEPMODE_ADC
//#define PM_SLEEPMODE_INVALID_RTC     PM_SLEEPMODE_PWR_DOWN
/* Watchdog is always running! not defining a level here will  */
//#define PM_SLEEPMODE_INVALID_WDT

/**
 * @brief   Available ports on the ATmega1281 family
 */
enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
    PORT_G = 6,       /**< port G */
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
