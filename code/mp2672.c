#include <stdio.h>
#include "hardware/i2c.h"

#define MP2672_ADDR 0x4B

enum
{
    MP2672_REG_BATT_SETTINGS = 0x0,
    MP2672_REG_BALANCE_CHARGE_SETTINGS,
    MP2672_REG_TIMER_SETTINGS,
    MP2672_REG_STATUS,
    MP2672_REG_FAULT
};

static uint8_t mp2672_read_register(uint8_t addr)
{
    uint8_t ret = 0xAA;
    
    i2c_write_blocking(i2c_default, MP2672_ADDR, &addr, 1, true);
    i2c_read_blocking(i2c_default, MP2672_ADDR, &ret, 1, false);

    return ret;
}

static void mp2672_write_register(uint8_t addr, uint8_t val)
{
    uint8_t data[] = {addr, val};
    i2c_write_blocking(i2c_default, MP2672_ADDR, data, 2, false);
}

void mp2672_init(void)
{
    // Nothing to do yet...
}

uint8_t mp2672_get_fault(void)
{
    return mp2672_read_register(MP2672_REG_FAULT);
}

uint8_t mp2672_get_status(void)
{
    return mp2672_read_register(MP2672_REG_STATUS);
}