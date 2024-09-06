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
#include "signal.h"

float first_order_filter(float prev_output, float new_input, float alpha)
{
    return new_input * alpha + prev_output * (1.0 - alpha);
}

float saturate(float signal, float lower_limit, float upper_limit)
{
    if(signal > upper_limit)
    {
        return upper_limit;
    }
    else if(signal < lower_limit)
    {
        return lower_limit;
    }

    return signal;
}

float derivative_saturation(float command, float prev_command, float max_accel_step, float max_decel_step, float max_command)
{
    float constrained_command;

    constrained_command = saturate(command, prev_command - max_decel_step, prev_command + max_accel_step);
    return saturate(constrained_command, 0.0, max_command);
}