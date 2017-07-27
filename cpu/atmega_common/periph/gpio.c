/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA

 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ATmega family
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 *
 * @}
 */


#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#define ENABLE_DEBUG (1)
#include "debug.h"

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)

/*
 * @brief     Define GPIO interruptions for an specific atmega CPU, by default
 *            2 (for small atmega CPUs)
 */
#if defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

#if defined(PCINT3_vect)
#define GPIO_PC_INT_NUMOF       (32U)
#elif defined(PCINT2_vect)
#define GPIO_PC_INT_NUMOF       (24U)
#elif defined(PCINT1_vect)
#define GPIO_PC_INT_NUMOF       (16U)
#elif defined(PCINT0_vect)
#define GPIO_PC_INT_NUMOF       (8U)
#endif

static gpio_isr_ctx_t config[GPIO_EXT_INT_NUMOF];

// TODO(rh): Make number configurable
static gpio_isr_ctx_t pcint[GPIO_PC_INT_NUMOF];

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/**
 * @brief     Extract the port number of the given pin
 */
static inline uint8_t _port_num(gpio_t pin)
{
    return (pin >> 4) & 0x0f;
}

/**
 * @brief     Generate the PORTx address of the give pin.
 */
static inline uint16_t _port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint16_t port_addr = port_num * GPIO_OFFSET_PIN_PIN;

    port_addr += GPIO_BASE_PORT_A;
    port_addr += GPIO_OFFSET_PIN_PORT;

#if defined (PORTG)
    if (port_num > PORT_G) {
        port_addr += GPIO_OFFSET_PORT_H;
    }
#endif
    return port_addr;
}

/**
 * @brief     Generate the DDRx address of the given pin
 */
static inline uint16_t _ddr_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x01);
}

/**
 * @brief     Generate the PINx address of the given pin.
 */
static inline uint16_t _pin_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x02);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    switch (mode) {
        case GPIO_OUT:
            _SFR_MEM8(_ddr_addr(pin)) |= (1 << _pin_num(pin));
            break;
        case GPIO_IN:
            _SFR_MEM8(_ddr_addr(pin)) &= ~(1 << _pin_num(pin));
            _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
            break;
        case GPIO_IN_PU:
            _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
            break;
        default:
            return -1;
    }

    return 0;
}

static inline void pcint_handler(uint8_t port_num, uint8_t pin_num)
{
    __enter_isr();
    pcint[port_num*8+pin_num].cb(pcint[port_num*8+pin_num].arg);
    __exit_isr();
}

static inline uint8_t pcint_port_pin(volatile uint8_t *reg) {
  uint8_t pin = 0;
  uint8_t val = *reg;
  while(val > 1) {
    val = val >> 1;
    pin++;
  }
  return pin;
}

// TODO(rh): Make this better!
//gpio_cb_t radio_cb = NULL;
//void *radio_arg = NULL;
#ifdef GPIO_PC_INT_NUMOF
/*
 * PCINT0 is always defined, if GPIO_PC_INT_NUMOF is defined
 */
/// Used for
#if 0
ISR(PCINT0_vect) {
  pcint_handler(0, pcint_port_pin(&PCMSK0));
}
#endif
#if defined(PCINT1_vect)
ISR(PCINT1_vect) {
  pcint_handler(1, pcint_port_pin(&PCMSK1));
}
#endif /* PCINT1_vect */
#if defined(PCINT2_vect)
ISR(PCINT2_vect) {
  pcint_handler(2, pcint_port_pin(&PCMSK2));
}
#endif /* PCINT2_vect */
#if defined(PCINT3_vect)
ISR(PCINT3_vect) {
  // DEBUG("PCINT3_vect %u\n", pcint_port_pin(&PCMSK3));
  pcint_handler(3, pcint_port_pin(&PCMSK3)); // pcint_port_pin(&PCMSK3)
  // radio_cb(radio_arg);
}
#endif /* PCINT3_vect */
#endif /* GPIO_PC_INT_NUMOF */

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    uint8_t pin_num = _pin_num(pin);
    uint8_t port_num = _port_num(pin);

    /// In case no external interrupt is available: enable a pin change interrupt
    if ((port_num == PORT_D && pin_num > 3)
#if defined (PORTE)
         || (port_num == PORT_E && pin_num < 4)
#endif
         || ((mode != GPIO_IN) && (mode != GPIO_IN_PU))) {
           switch(port_num) {
             case 0 :
               PCMSK0 |= (1 << pin_num);
               PCICR |= (1 << PCIE0);
             break;
             case 1:
               PCMSK1 |= (1 << pin_num);
               PCICR |= (1 << PCIE1);
             break;
             case 2 :
               PCMSK2 |= (1 << pin_num);
               PCICR |= (1 << PCIE2);
             break;
#ifdef PCIE3
             case 3 :
               PCMSK3 |= (1 << pin_num);
               PCICR |= (1 << PCIE3);
             break;
#endif
           }

           DEBUG("gpio.c: pcint for port: %u, pin: %u, mode: %u\n", port_num, pin_num, mode);

           pcint[port_num*8+pin_num].cb = cb;
           pcint[port_num*8+pin_num].arg = arg;

           //radio_cb = cb;
           //radio_arg = arg;
        return 0;
    }

    gpio_init(pin, mode);

    /* clear global interrupt flag */
    cli();

    EIMSK |= (1 << pin_num);

    /* configure the flank */
    switch (flank) {
        case GPIO_RISING:
            if (pin_num < 4) {
                EICRA |= (3 << pin_num * 2);
            }
#if defined(EICRB)
            else {
                EICRB |= (3 << (pin_num * 2) % 4);
            }
#endif
            break;
        case GPIO_FALLING:
            if (pin_num < 4) {
                EICRA |= (2 << pin_num * 2);
            }
#if defined(EICRB)
            else {
                EICRB |= (2 << (pin_num * 2) % 4);
            }
#endif
            break;
        case GPIO_BOTH:
            if (pin_num < 4) {
                EICRA |= (1 << pin_num * 2);
            }
#if defined(EICRB)
            else {
                EICRB |= (1 << (pin_num * 2) % 4);
            }
#endif
            break;
        default:
            return -1;
    };

    /* set callback */
    config[pin_num].cb = cb;
    config[pin_num].arg = arg;

    /* set global interrupt flag */
    sei();

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EIMSK |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EIMSK &= ~(1 << _pin_num(pin));
}

int gpio_read(gpio_t pin)
{
    return (_SFR_MEM8(_pin_addr(pin)) & (1 << _pin_num(pin)));
}

void gpio_set(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

static inline void irq_handler(uint8_t pin_num)
{
    __enter_isr();
    config[pin_num].cb(config[pin_num].arg);
    __exit_isr();
}

ISR(INT0_vect, ISR_BLOCK)
{
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(INT1_vect, ISR_BLOCK)
{
    irq_handler(1); /**< predefined interrupt pin */
}

#if defined(INT2_vect)
ISR(INT2_vect, ISR_BLOCK)
{
    irq_handler(2); /**< predefined interrupt pin */
}
#endif

#if defined(INT3_vect)
ISR(INT3_vect, ISR_BLOCK)
{
    irq_handler(3); /**< predefined interrupt pin */
}
#endif

#if defined(INT4_vect)
ISR(INT4_vect, ISR_BLOCK)
{
    irq_handler(4); /**< predefined interrupt pin */
}
#endif

#if defined(INT5_vect)
ISR(INT5_vect, ISR_BLOCK)
{
    irq_handler(5); /**< predefined interrupt pin */
}
#endif

#if defined(INT6_vect)
ISR(INT6_vect, ISR_BLOCK)
{
    irq_handler(6); /**< predefined interrupt pin */
}
#endif

#if defined(INT7_vect)
ISR(INT7_vect, ISR_BLOCK)
{
    irq_handler(7); /**< predefined interrupt pin */
}
#endif
