#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ltc2943.h"
#include "mp2672.h"

void monitor_charge(uint32_t sec)
{
    for(uint32_t k = 0; k < sec; k++)
    {
        float u = ltc2943_read_voltage();
        float i = ltc2943_read_current();
        uint8_t regs[5];
        mp2672_get_regs(regs);

        // Skip bogus first read
        if(k == 0) continue;

        // Status
        uint8_t raw_status = mp2672_get_status();
        uint8_t status = (raw_status & 0x30) >> 4;
        uint8_t batt_float = (raw_status & 0x4) >> 2;
        uint8_t therm = (raw_status & 0x2) >> 1;

        // Fault
        uint8_t raw_fault = mp2672_get_fault();
        uint8_t therm_fault = (raw_fault >> 5) & 0x1;
        uint8_t ntc_fault = (raw_fault & 0x7);

        printf("%f %f %d %d %d %d %d", u, i, status, batt_float, therm, therm_fault, ntc_fault);

        for(uint8_t i = 0; i < sizeof(regs); i++)
        {
            printf(" 0x%02X", regs[i]);
        }

        printf("\n");

        sleep_ms(1000);
    }
}

int main(void)
{
    stdio_init_all();

    i2c_init(i2c_default, 100000); // 100kHz

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    ltc2943_init();
    mp2672_init();
    
    // Wait for serial console to be connected
    while(!stdio_usb_connected());

    printf("# voltage current status battery_present thermal_regulation thermal_fault ntc_fault reg0 .. reg5\n");

    while(true) 
    {
        mp2672_enable_charge(false);
        printf("# Charge disabled\n");
        monitor_charge(30);
        
        mp2672_enable_charge(true);
        printf("# Charge enabled\n");
        monitor_charge(30);
        
    }

    return 0;
}