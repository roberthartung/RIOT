/*
 * board.c - Board initiazilation for the INGA sensor node
 * Copyright (C) 2016 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cpu.h"
#include "board.h"
#include "uart_stdio.h"

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void board_init(void)
{
  // Disable watchdog
  WDTCSR &= ~_BV(WDIE);

  /* initialize STDIO over UART */
  uart_stdio_init();
  stdout = &uart_stdout;
  stdin = &uart_stdin;
  puts("\f");

  /* initialize the CPU */
  cpu_init();

  irq_enable();
}

static int uart_putchar(char c, FILE *stream)
{
  (void) stream;
  uart_stdio_write(&c, 1);
  return 0;
}

static int uart_getchar(FILE *stream)
{
  (void) stream;
  char c;
  uart_stdio_read(&c, 1);
  return (int)c;
}
