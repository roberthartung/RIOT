/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_pm_layered
 * @{
 *
 * @file
 * @brief       Platform-independent power management code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "pm_layered.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_NUM_MODES
#error PM_NUM_MODES must be defined in periph_cpu.h!
#endif

#if PM_NUM_MODES > 8
  #error pm_layer does not work for more than 4 modes
#elif PM_NUM_MODES > 4
  #ifndef CPU_ATMEGA1284P
    #error More than 4 modes are efficient and should be avoided. Change number of modes or add define here
  #endif
#endif

#ifndef PM_BLOCKER_INITIAL
  #define PM_BLOCKER_INITIAL { .val = 0 }
#endif

/**
 * @brief Power Management mode typedef
 */
typedef union {
#if PM_NUM_MODES > 4
    uint64_t val;
    uint8_t val_u8[8];
#else
    uint32_t val;
    uint8_t val_u8[4];
#endif
} pm_blocker_t;

/**
 * @brief Global variable for keeping track of blocked modes
 */
volatile pm_blocker_t pm_blocker = PM_BLOCKER_INITIAL;

void pm_set_lowest(void)
{
    pm_blocker_t blocker = (pm_blocker_t) pm_blocker;
    unsigned mode = PM_NUM_MODES;
    while (mode) {
        if (blocker.val_u8[mode-1]) {
            break;
        }
        mode--;
    }

    /* set lowest mode if blocker is still the same */
    unsigned state = irq_disable();
    if (blocker.val == pm_blocker.val) {
        DEBUG("pm: setting mode %u\n", mode);
        pm_set(mode);
    }
    else {
        DEBUG("pm: mode block changed\n");
    }
    irq_restore(state);
}

void pm_block(unsigned mode)
{
    assert(pm_blocker.val_u8[mode] != 255);

    unsigned state = irq_disable();
    pm_blocker.val_u8[mode]++;
    irq_restore(state);
}

void pm_unblock(unsigned mode)
{
    assert(pm_blocker.val_u8[mode] > 0);

    unsigned state = irq_disable();
    pm_blocker.val_u8[mode]--;
    irq_restore(state);
}

void __attribute__((weak)) pm_off(void)
{
    pm_blocker.val = 0;
    pm_set_lowest();
    while(1);
}
