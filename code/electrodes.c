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
#include "hardware/adc.h"
#include "electrodes.h"

void electrodes_init(void)
{
    adc_init();

    adc_gpio_init(EL1_PIN);
    adc_gpio_init(EL2_PIN);
}

void electrodes_read(float *el1, float *el2)
{
    // Read electrode 1 value
    adc_select_input(EL1_CHANNEL);
    *el1 = adc_read() * VREF / (1 << 12);
    
    // Read electrode 2 value
    adc_select_input(EL2_CHANNEL);
    *el2 = adc_read() * VREF / (1 << 12);
}