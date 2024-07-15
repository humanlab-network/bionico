#include "led.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

void vibrator_init(void)
{
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1000);
    pwm_config_set_wrap(&config, 999);
    
    gpio_set_function(VIBRATOR_PIN, GPIO_FUNC_PWM);
    pwm_init(pwm_gpio_to_slice_num(VIBRATOR_PIN), &config, true);
    pwm_set_gpio_level(VIBRATOR_PIN, 0);
}

void vibrator_set_value(uint16_t vib)
{
    pwm_set_gpio_level(VIBRATOR_PIN, vib);
}