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
#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_PWM 100

void motor_init();

void motor_set_command(int8_t pwm);

float motor_read_current(void);

#endif