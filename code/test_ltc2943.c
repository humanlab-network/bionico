#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ltc2943.h"

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

    ltc2943_init();

    while(1)
    {
        printf("%f %f\n", ltc2943_read_voltage(), ltc2943_read_current());
        sleep_ms(1000);
    }

    return 0;
}