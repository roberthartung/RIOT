/*
 * Copyright (C) 2017 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief A test application to check weather the sleep is correctly woken up
 *        after being put to sleep
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"

/// for __enter_isr and __exit_isr
#include "cpu.h"
#include "irq.h"
#include "board.h"
#include "periph/rtt.h"
#include "periph/uart.h"
#include "pm_layered.h"
#include "thread.h"

static volatile bool uart_enabled = true;
static volatile int cnt = 0;
/* Counter if the sleep state was ok */
static volatile int cnt_ok = 0;

static void cb(void *arg)
{
    cnt++;
    /* Switch uart state every RTT alarm */
    if(uart_enabled) {
        /* If the uart was enabled, disable it and check state */
        if(pm_sleep_flag == true && (pm_sleep_mode - 1) == PM_SLEEPMODE_INVALID_UART0 ) {
            cnt_ok++;
        }
        /* power off uart and set flag */
        uart_poweroff(0);
        uart_enabled = !uart_enabled;
    } else {
        /* if the uart was disabled, we woke up from sleep mode where rtt was allowed */
        if(pm_sleep_flag == true && (pm_sleep_mode - 1)== PM_SLEEPMODE_INVALID_TIMER2 ) {
            cnt_ok++;
        }
        /* power on uart and set flag */
        uart_poweron(0);
        uart_enabled = !uart_enabled;
    }

    /* Wakeup thread */
    thread_wakeup(2);
    /* Check if we need to switch context */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

int main(void)
{
    rtt_init();

    puts("[Start] Test should take about 12 seconds.");

    while(1) {
        rtt_set_alarm(1, cb, NULL);
        if(uart_enabled) {
            printf("before sleep: cnt=%d sleep_state_ok=%d\n", cnt, cnt_ok);
        }
        thread_sleep();
        if(uart_enabled) {
            if(cnt == cnt_ok && cnt > 6) {
                puts("[Succeed]");
                break;
            }
            printf("after sleep: cnt=%d sleep_state_ok=%d\n", cnt, cnt_ok);
        }
    }

    return 0;
}
