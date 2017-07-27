/*
 * Copyright (C) 2017 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega1284p
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include <avr/sleep.h>
#include "irq.h"
#include "avr/io.h"
#include "periph_cpu.h"
#include "pm_layered.h"

void pm_set(unsigned mode)
{
    switch(mode) {
        case PM_SLEEPMODE_EXT_STANDBY :
            set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
        break;
        case PM_SLEEPMODE_STANDBY :
            set_sleep_mode(SLEEP_MODE_STANDBY);
        break;
        case PM_SLEEPMODE_PWR_SAVE :
            set_sleep_mode(SLEEP_MODE_PWR_SAVE);
        break;
        case PM_SLEEPMODE_PWR_DOWN :
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        break;
        case PM_SLEEPMODE_ADC :
            set_sleep_mode(SLEEP_MODE_ADC);
        break;
        case PM_SLEEPMODE_IDLE :
            set_sleep_mode(SLEEP_MODE_IDLE);
        break;
        default :
            DEBUG("pm.c: sleep mode out or range (run mode?)\n");
            // no sleep here!
            return;
    }

    DEBUG("pm_set=%d\n", mode);
    sleep_enable();
    pm_sleep_mode = mode;
    pm_sleep_flag = true;
    /* irq_enable() and sleep_cpu() should follow each other directly, so no
      interrupt can come in between. Problem: There might be a context switch above */
    irq_enable();
    sleep_cpu();
    sleep_disable();
}
