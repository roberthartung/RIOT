/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef INGA_COMMON_BOARD_H_
#define INGA_COMMON_BOARD_H_

#include "cpu.h"
#include "periph_conf.h"
/* For GPIO_IN */
#include "periph/gpio.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Not yet any specific board configurations */

#define BTN0_PIN            GPIO_PIN(1,2) //PB2
#define BTN0_MODE           GPIO_IN

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_BOARD_H_ */
