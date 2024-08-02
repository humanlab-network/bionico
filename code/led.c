#include "led.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define LED_INTENSITY (500)

void led_init(void)
{
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1000);
    pwm_config_set_wrap(&config, 999);
    
    for(uint8_t i = 0; i < 2; i++)
    {
        gpio_set_function(LED_PIN(i), GPIO_FUNC_PWM);
        pwm_init(pwm_gpio_to_slice_num(LED_PIN(i)), &config, true);
        pwm_set_gpio_level(LED_PIN(i), 0);
    }
}

void led_set_state(led_state_t state)
{
    switch(state)
    {
        case LEDS_OFF:
            pwm_set_gpio_level(LED_PIN(0), 0);
            pwm_set_gpio_level(LED_PIN(1), 0);
            break;

        case LED1_ON:
            pwm_set_gpio_level(LED_PIN(0), 0);
            pwm_set_gpio_level(LED_PIN(1), LED_INTENSITY);
            break;

        case LED2_ON:
            pwm_set_gpio_level(LED_PIN(0), LED_INTENSITY);
            pwm_set_gpio_level(LED_PIN(1), 0);
            break;
    }
}