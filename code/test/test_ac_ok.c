#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "mp2672.h"
#include "led.h"

int main(void)
{
    stdio_init_all();

    i2c_init(i2c_default, 100000); // 100kHz

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    led_init();
    mp2672_init();

    while(1)
    {
        led_set_state(mp2672_is_ac_ok()?LED2_ON:LED1_ON);
    }

    return 0;
}