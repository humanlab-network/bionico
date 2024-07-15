#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "mp2672.h"

int main(void)
{
    stdio_init_all();

    i2c_init(i2c_default, 100000); // 100kHz

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    // Wait for serial console to be connected
    while(!stdio_usb_connected());

    mp2672_init();

    while(1)
    {
        printf("Fault: %02X\n", mp2672_get_fault());
        printf("Status: %02X\n", mp2672_get_status());
        printf("\n");
        sleep_ms(1000);
    }

    return 0;
}