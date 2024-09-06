/* 
 * Bionico code is the firmware source code embedded in the electronic board
 * dedicated to the prothestic hand [bionicohand](https://bionico.org/)
 * Copyright 2024 INRIA
 * Contributors: see AUTHORS file
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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