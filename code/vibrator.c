/* 
 * Bionico code is the firmware source code embedded in the electronic board
 * dedicated to the prothestic hand [bionicohand](https://bionico.org/)
 * Copyright 2024 INRIA
 * Contributors: see AUTHORS file
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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