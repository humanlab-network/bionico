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
#ifndef LTC2943_H_ 
#define LTC2943_H_

#include <stdint.h>

void ltc2943_init(void);

float ltc2943_read_voltage(void);
float ltc2943_read_current(void);
float ltc2943_read_temperature(void);

float ltc2943_read_charge(void);
void ltc2943_set_charge(float charge);

uint8_t ltc2943_read_status(void);

#endif