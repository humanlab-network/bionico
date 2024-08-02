#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include "mp2672.h"

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

static void sw_reset(void)
{
    uint8_t r;

    // Reset chip
    mp2672_write_register(MP2672_REG_TIMER_SETTINGS, 0x8);

    // Wait for reset to complete 
    do
    {
        r = mp2672_read_register(MP2672_REG_TIMER_SETTINGS);
        sleep_ms(10);
    } while(r & 0x8);
}

void mp2672_init(void)
{
    // Initialize ACOK pin
    gpio_init(AC_OK_PIN);
    gpio_set_dir(AC_OK_PIN, GPIO_IN);
    gpio_pull_up(AC_OK_PIN);
}

void mp2672_configure(void)
{
    sw_reset();

    // Set charge current to 500mA (if Iset is 500mA)
    mp2672_write_register(MP2672_REG_BALANCE_CHARGE_SETTINGS, 0x8F);

    // Disable I2C watchdog to avoid resetting the registers after 60s
    mp2672_write_register(MP2672_REG_TIMER_SETTINGS, 0x83);

    // Disable charge by default
    mp2672_enable_charge(false);
}

bool mp2672_is_ac_ok(void)
{
    return !gpio_get(AC_OK_PIN);
}

uint8_t mp2672_get_fault(void)
{
    return mp2672_read_register(MP2672_REG_FAULT);
}

uint8_t mp2672_get_status(void)
{
    return mp2672_read_register(MP2672_REG_STATUS);
}

void mp2672_enable_charge(bool enable)
{
    #define CHARGE_ENABLE_BIT (1 << 4)

    uint8_t settings = mp2672_read_register(MP2672_REG_BATT_SETTINGS);

    if(enable)
    {
        settings |= CHARGE_ENABLE_BIT;
    }
    else
    {
        settings &= ~CHARGE_ENABLE_BIT;
    }

    mp2672_write_register(MP2672_REG_BATT_SETTINGS, settings);
}

void mp2672_get_regs(uint8_t *reg)
{
    for(uint8_t i = 0; i <= 4; i++)
    {
        reg[i] = mp2672_read_register(i);
    }
}