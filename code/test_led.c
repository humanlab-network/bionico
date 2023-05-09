#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main(void)
{
    stdio_init_all();

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1000);
    pwm_config_set_wrap(&config, 999);

    for(uint8_t i = 0; i < 8; i++)
    {
        gpio_set_function(LED_PIN(i), GPIO_FUNC_PWM);
        pwm_init(pwm_gpio_to_slice_num(LED_PIN(i)), &config, true);
        pwm_set_gpio_level(LED_PIN(i), 1000);
    }

    while(true)
    {
        for(int8_t i = 1; i < 8; i += 2)
        {
            printf("LED %d ON\n", i);
            pwm_set_gpio_level(LED_PIN(i), 900);
            sleep_ms(500);
            
            printf("LED %d ON\n", i - 1);
            pwm_set_gpio_level(LED_PIN(i - 1), 980);
            sleep_ms(500);
            
            printf("LED %d OFF\n", i);
            pwm_set_gpio_level(LED_PIN(i), 1000);
            sleep_ms(500);
        }
        
        for(int8_t i = 7; i >= 0; i--)
        {
            printf("LED %d OFF\n", i);
            pwm_set_gpio_level(LED_PIN(i), 1000);
        }

        sleep_ms(1000);
    }
    
    /////////////////////////////////

    for(uint8_t i = 0; i < 8; i++)
    {
        gpio_init(LED_PIN(i));
        gpio_set_dir(LED_PIN(i), GPIO_OUT);
        gpio_put(LED_PIN(i), 0);
    }

    while(true)
    {
        for(int8_t i = 0; i < 8; i++)
        {
            printf("LED %d ON\n", i);
            gpio_put(LED_PIN(i), 1);
            sleep_ms(500);
        }
        
        for(int8_t i = 7; i >= 0; i--)
        {
            printf("LED %d OFF\n", i);
            gpio_put(LED_PIN(i), 0);
            sleep_ms(500);
        }
    }

    return 0;
}
