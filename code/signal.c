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