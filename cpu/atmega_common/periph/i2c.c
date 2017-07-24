/*
 * Copyright (C) 2017 TU Braunschweig, IBR, Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  cpu_atmega_common
 * @{
 * @file
 * @brief       Low-level I2C driver implementation
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C_START        0x08
#define I2C_REP_START    0x10
#define I2C_MT_SLA_ACK   0x18
#define I2C_MT_DATA_ACK  0x28
#define I2C_MR_SLA_ACK   0x40
#define I2C_MR_DATA_ACK  0x50
#define I2C_MR_DATA_NACK 0x58

static int8_t _start(i2c_t dev, uint8_t addr, uint8_t rw_flag, uint8_t rep);
static void _stop(i2c_t dev);
static inline int8_t _write(i2c_t dev, uint8_t data);
static inline int8_t _write_bytes(i2c_t dev, const uint8_t *data, int length);
static inline int8_t _read(i2c_t dev, uint8_t *data, uint8_t ack);
static inline int8_t _read_bytes(i2c_t dev, uint8_t *data, int length);

/* guard file in case no I2C device is defined */
#if I2C_NUMOF
/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] = {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT
#endif
// #if I2C_1_EN
//     ,[I2C_1] = MUTEX_INIT
// #endif
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
  /// DISABLE I2C MASTER
  i2c_poweroff(dev);

  /// Resets the I2C prescaler
  TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
  /// 8000000÷(16+2×2×4^0) -> 8000000÷(16+2×)TWBR×4^(TWPS))
  switch (speed) {
      case I2C_SPEED_NORMAL:
          TWBR = 32;
          break;
      case I2C_SPEED_FAST:
          TWBR = 2;
          break;
      case I2C_SPEED_HIGH:
          TWBR = 1;
          break;
      default:
          DEBUG("BAD BAUDRATE\n");
          return -2;
  }

  /// ENABLE I2C MASTER
  i2c_poweron(dev);

  return 0;
}

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
  //DEBUG("i2c.c: i2c_read_bytes: a=0x%02X l=%d\n", address, length);
    /* start transmission and send slave address */
    if (_start(dev, address, I2C_FLAG_READ, 0) < 0) {
        DEBUG("_start failed\n");
        return 0;
    }
    /* read data to register */
    if (_read_bytes(dev, data, length) < 0) {
        DEBUG("_read_bytes failed\n");
        return 0;
    }
    _stop(dev);
    /* return number of bytes sent */
    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
  //DEBUG("i2c.c: i2c_read_regs: a=0x%02X r=0x%02X l=%d\n", address, reg, length);
    /* start transmission and send slave address */
    if (_start(dev, address, I2C_FLAG_WRITE, 0) < 0) {
        DEBUG("_start failed\n");
        return 0;
    }
    /* send register address/command and wait for complete transfer to
     * be finished */
    if (_write_bytes(dev, &reg, 1) < 0) {
        DEBUG("_write_bytes(reg) failed\n");
        return 0;
    }

    /// 1: repeated start!
    if (_start(dev, address, I2C_FLAG_READ, 1) < 0) {
        DEBUG("_start(rep) failed\n");
        return 0;
    }

    if(_read_bytes(dev, data, length) < 0) {
      return 0;
    }

    _stop(dev);
    return length;
    //return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
  //DEBUG("i2c.c: i2c_write_bytes: a=0x%02X l=%d\n", address, length);

    if (_start(dev, address, I2C_FLAG_WRITE, 0) < 0) {
        DEBUG("_start failed\n");
        return 0;
    }
    if (_write_bytes(dev, data, length) < 0) {
        DEBUG("_write_bytes failed\n");
        return 0;
    }
    _stop(dev);
    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    //DEBUG("i2c.c: i2c_write_regs: a=%02X r=%02X l=%d\n", address, reg, length);
    /* start transmission and send slave address */
    if (_start(dev, address, I2C_FLAG_WRITE, 0) < 0) {
      DEBUG("_start failed\n");
        return 0;
    }
    /* send register address and wait for complete transfer to be finished */
    if (_write_bytes(dev, &reg, 1) < 0) {
      DEBUG("_write_bytes(reg) failed\n");
        return 0;
    }
    /* write data to register */
    if (_write_bytes(dev, data, length) < 0) {
      DEBUG("_write_bytes(data) failed\n");
        return 0;
    }
    /* finish transfer */
    _stop(dev);
    return length;
}

static int8_t _start(i2c_t dev, uint8_t addr, uint8_t rw_flag, uint8_t rep) {
  uint16_t i = 0;

  //MEGA_PRR &= ~(1 << PRTWI);
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    if( i++ > 800 ) {
      DEBUG("i2c.c: _start timeout ERROR\n");
      return -3;
    }
  }

  if ((TWSR & 0xF8) != ((!rep) ? I2C_START : I2C_REP_START)) {
    DEBUG("i2c.c: _start start/rep_start ERROR\n");
    return -1;
  }

  TWDR = (addr<<1) | rw_flag;
  //DEBUG("i2c.c: _start: 0x%02X rep=%d\n", TWDR, rep);
  TWCR = (1 << TWINT) | (1 << TWEN);
  i = 0;

  while (!(TWCR & (1 << TWINT))) {
    if( i++ > 800 ) {
      DEBUG("i2c.c: _start timeout ERROR\n");
      return -4;
    }
  }
  if (!(((TWSR & 0xF8) == I2C_MT_SLA_ACK) || ((TWSR & 0xF8) == I2C_MR_SLA_ACK))) {
    return -2;
  }

  return 0;
}

static void _stop(i2c_t dev) {
  //DEBUG("i2c.c: _stop\n");
  uint16_t i = 0;

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO)) {
    if (i++ > 800) {
      DEBUG("i2c.c: _stop timeout ERROR\n");
      return;
    }
  }

  TWCR &= ~(1 << TWEN);

  //MEGA_PRR |= (1 << PRTWI);
  /// TODO(rh): PINS
  DDRC &= ~((1 << PC0) | (1 << PC1));
  PORTC |= ((1 << PC0) | (1 << PC1));
}

static inline int8_t _write_bytes(i2c_t dev, const uint8_t *data, int length) {
  uint16_t tmp_data_length = length;
  uint16_t buffer_counter = 0;
  while(tmp_data_length--) {
    int8_t ret = _write(dev, data[buffer_counter++]);
    if(ret != 0) {
      return ret;
    }
  }
  return 0;
}

static inline int8_t _write(i2c_t dev, uint8_t data) {
  //DEBUG("i2c.c: _write: 0x%02X\n", data);
  uint16_t i = 0;

  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);

  while (!(TWCR & (1 << TWINT))) {
    if( i++ > 800 ) {
      DEBUG("i2c.c: _write: Timeout\n");
      return -2;
    }
  }

  if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) {
    DEBUG("i2c.c: _write: ACK expected\n");
    return -1;
  }

  return 0;
}

static inline int8_t _read_bytes(i2c_t dev, uint8_t *data, int length) {
  uint16_t tmp_data_length = length;
  uint16_t buffer_counter = 0;
  while(tmp_data_length-- > 0) {
    /// Ack only if more than one byte to read
    uint8_t ack = tmp_data_length == 0 ? 0 : 1;
    int8_t ret = _read(dev, &data[buffer_counter++], ack);
    if(ret != 0) {
      return ret;
    }
  }
  return 0;
}

static inline int8_t _read(i2c_t dev, uint8_t *data, uint8_t ack) {
  uint16_t i = 0;
  TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
  while (!(TWCR & (1 << TWINT))) {
    if (i++ > 800) {
      DEBUG("_read: Timeout Error\n");
      return -1;
    }
  }
  if ((TWSR & 0xF8) != (ack ? I2C_MR_DATA_ACK : I2C_MR_DATA_NACK)) {
    DEBUG("_read: ACK/NACK Error\n");
    return -2;
  }
  //DEBUG("i2c.c: _read 0x%02X ack=%d\n", TWDR, ack);
  *data = TWDR;
  return 0;
}

void i2c_poweron(i2c_t dev) {
  MEGA_PRR &= ~(1 << PRTWI);
}

void i2c_poweroff(i2c_t dev) {
  MEGA_PRR |= (1 << PRTWI);
}

#endif /* I2C_NUMOF */
