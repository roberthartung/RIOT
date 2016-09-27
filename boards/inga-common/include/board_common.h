/*
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef INGA_COMMON_BOARD_COMMON_H_
#define INGA_COMMON_BOARD_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AT86RF2XX_PARAMS_BOARD {.spi = SPI_0, \
                                .spi_speed = AT86RF2XX_PARAM_SPI_SPEED, \
                                .cs_pin = GPIO_PIN(PORT_B, 4),\
                                .int_pin = GPIO_PIN(PORT_D, 6),\
                                .sleep_pin = GPIO_PIN(PORT_B, 3),\
                                .reset_pin = GPIO_PIN(PORT_B, 1)}

/**
 * @brief   Override default baudrate for STDIO
 * @{
 */
#ifndef UART_STDIO_BAUDRATE
#define UART_STDIO_BAUDRATE         (19200)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_BOARD_COMMON_H_ */
