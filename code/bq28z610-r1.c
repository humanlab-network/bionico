#include <stdio.h>
#include "hardware/i2c.h"

#define BQ28Z610_R1_ADDR 0x55

#define BQ28Z610_R1_REG_TEMP     0x06
#define BQ28Z610_R1_REG_VOLT     0x08
#define BQ28Z610_R1_REG_FLAGS    0x0A
#define BQ28Z610_R1_REG_INSTCURR 0x0C
#define BQ28Z610_R1_REG_RM       0x10
#define BQ28Z610_R1_REG_MAC_CMD  0x3E

uint16_t bq28z610_r1_read_register(uint8_t addr) // TODO: static
{
    uint8_t data[2] = {0xAA, 0xBB};
    
    i2c_write_blocking(i2c_default, BQ28Z610_R1_ADDR, &addr, 1, true);
    i2c_read_blocking(i2c_default, BQ28Z610_R1_ADDR, data, 2, false);

    return (data[1] << 8) | data[0];
}

static void bq28z610_r1_write_register(uint8_t addr, uint16_t val)
{
    uint8_t data[] = {addr, val & 0xFF, val >> 8};
    i2c_write_blocking(i2c_default, BQ28Z610_R1_ADDR, data, 3, false);
}

static uint16_t bq28z610_r1_control_read_data(uint8_t *data, uint8_t len)
{
    uint8_t addr = BQ28Z610_R1_REG_MAC_CMD;
    i2c_write_blocking(i2c_default, BQ28Z610_R1_ADDR, &addr, 1, true);
    i2c_read_blocking(i2c_default, BQ28Z610_R1_ADDR, data, len, false);
}

uint16_t bq28z610_r1_control_read(uint16_t cmd, uint8_t *data, uint8_t len) // TODO: static
{
    bq28z610_r1_write_register(BQ28Z610_R1_REG_MAC_CMD, cmd);
    return bq28z610_r1_control_read_data(data, len);
}

void bq28z610_r1_init(void)
{
    // Nothing to do yet...
}

float bq28z610_r1_get_voltage(void)
{
    uint16_t volt = bq28z610_r1_read_register(BQ28Z610_R1_REG_VOLT);
    return volt * 1e-3;
}

float bq28z610_r1_get_current(void)
{
    int16_t curr = bq28z610_r1_read_register(BQ28Z610_R1_REG_INSTCURR);
    return curr * 1e-3;
}

float bq28z610_r1_get_temperature(void)
{
    uint16_t temp = bq28z610_r1_read_register(BQ28Z610_R1_REG_TEMP);
    return temp * 0.1 - 273.15;
}

float bq28z610_r1_get_remaining_capacity(void)
{
    uint16_t rm = bq28z610_r1_read_register(BQ28Z610_R1_REG_RM);
    return rm * 1e-3;
}

uint16_t bq28z610_r1_get_status(void)
{
    return bq28z610_r1_read_register(BQ28Z610_R1_REG_FLAGS);
}