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
#include "thermal_model.h"

#include "control_config.h"

void run_thermal_model(float *rotor_temp, float prev_rotor_temp, float *stator_temp, float prev_stator_temp, float motor_current)
{
    float dT_rotor = motor_current * motor_current * (R_MOT * (RTH_ROT / TAU_ROT)) - (prev_rotor_temp - prev_stator_temp) / TAU_ROT;
    *rotor_temp = prev_rotor_temp + dT_rotor * PERIOD_SEC;

    float dT_stator = ((RTH_ST / TAU_ST) / RTH_ROT) * (prev_rotor_temp - prev_stator_temp) - (prev_stator_temp - T_EXT) / TAU_ST;
    *stator_temp = prev_stator_temp + dT_stator * PERIOD_SEC;
}