#include <stdio.h>
#include "hardware/i2c.h"
#include "ltc2943.h"

#define LTC2943_ADDR 0x64

#define RSENSE 0.015 // Ohm

enum
{
    LTC2943_COULOMB_PRESCALER_1 = 0 << 3,
    LTC2943_COULOMB_PRESCALER_4 = 1 << 3,
    LTC2943_COULOMB_PRESCALER_16 = 2 << 3,
    LTC2943_COULOMB_PRESCALER_64 = 3 << 3,
    LTC2943_COULOMB_PRESCALER_256 = 4 << 3,
    LTC2943_COULOMB_PRESCALER_1024 = 5 << 3,
    LTC2943_COULOMB_PRESCALER_4096 = 6 << 3
};

enum
{
    LTC2943_STATUS = 0x00,
    LTC2943_CONTROL,
    LTC2943_ACC_CHRG_MSB,
    LTC2943_ACC_CHRG_LSB,
    LTC2943_CHRG_THRES_HI_MSB,
    LTC2943_CHRG_THRES_HI_LSB,
    LTC2943_CHRG_THRES_LO_MSB,
    LTC2943_CHRG_THRES_LO_LSB,
    LTC2943_VOLT_MSB,
    LTC2943_VOLT_LSB,
    LTC2943_VOLT_THRES_HI_MSB,
    LTC2943_VOLT_THRES_HI_LSB,
    LTC2943_VOLT_THRES_LO_MSB,
    LTC2943_VOLT_THRES_LO_LSB,
    LTC2943_CURRENT_MSB,
    LTC2943_CURRENT_LSB,
    LTC2943_CURRENT_THRES_HI_MSB,
    LTC2943_CURRENT_THRES_HI_LSB,
    LTC2943_CURRENT_THRES_LO_MSB,
    LTC2943_CURRENT_THRES_LO_LSB,
    LTC2943_TEMP_MSB,
    LTC2943_TEMP_LSB,
    LTC2943_TEMP_THRES_HI,
    LTC2943_TEMP_THRES_LO
};

static uint16_t coulomb_prescaler = 4096;

static uint8_t ltc2943_read_register_8(uint8_t addr)
{
    uint8_t ret = 0xAA;
    
    i2c_write_blocking(i2c_default, LTC2943_ADDR, &addr, sizeof(addr), true);
    i2c_read_blocking(i2c_default, LTC2943_ADDR, &ret, sizeof(ret), false);

    return ret;
}

static uint16_t ltc2943_read_register_16(uint8_t addr)
{
    uint8_t ret[] = {0xAA, 0xAA};
    
    i2c_write_blocking(i2c_default, LTC2943_ADDR, &addr, sizeof(addr), true);
    i2c_read_blocking(i2c_default, LTC2943_ADDR, ret, sizeof(ret), false);

    return (ret[0] << 8) | ret[1];
}

static void ltc2943_write_register_8(uint8_t addr, uint8_t val)
{
    uint8_t data[] = {addr, val};
    i2c_write_blocking(i2c_default, LTC2943_ADDR, data, sizeof(data), false);
}

static void ltc2943_write_register_16(uint8_t addr, uint16_t val)
{
    uint8_t data[] = {addr, (val >> 8) & 0xFF, val & 0xFF};
    i2c_write_blocking(i2c_default, LTC2943_ADDR, data, sizeof(data), false);
}

static void ltc2943_set_coulomb_prescaler(uint8_t p)
{
    uint8_t c = ltc2943_read_register_8(LTC2943_CONTROL);
    ltc2943_write_register_8(LTC2943_CONTROL, (c & 0xC7) | p);
    coulomb_prescaler = 1 << (p >> 2);
}

void ltc2943_init(void)
{
    ltc2943_write_register_8(LTC2943_CONTROL, 0xC0);

    ltc2943_set_coulomb_prescaler(LTC2943_COULOMB_PRESCALER_64);
}

float ltc2943_read_voltage(void)
{
    uint16_t r = ltc2943_read_register_16(LTC2943_VOLT_MSB);

    return 23.6 * r / 65535.; 
}

float ltc2943_read_current(void)
{
    uint16_t r = ltc2943_read_register_16(LTC2943_CURRENT_MSB);

    return 60e-3 / RSENSE * (r - 32767) / 32767;
}

float ltc2943_read_temperature(void)
{
    uint16_t r = ltc2943_read_register_16(LTC2943_TEMP_MSB);

    return 510. * r / 65535. - 273.15; 
}

float ltc2943_read_charge(void)
{
    uint16_t r = ltc2943_read_register_16(LTC2943_ACC_CHRG_MSB);
    float q_lsb = 0.34 * 0.05 / RSENSE * coulomb_prescaler / 4096.;
    return q_lsb * r;
}

void ltc2943_set_charge(float charge)
{
    float q_lsb = 0.34 * 0.05 / RSENSE * coulomb_prescaler / 4096.;

    ltc2943_write_register_16(LTC2943_ACC_CHRG_MSB, (uint16_t)(charge / q_lsb));
}

uint8_t ltc2943_read_status(void)
{
    return ltc2943_read_register_8(LTC2943_STATUS);
}