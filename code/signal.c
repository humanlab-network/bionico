#include "signal.h"

// TODO remove pointers

float first_order_filter(float *prev_output, float *new_input, float alpha)
{
    return (*new_input * alpha + *prev_output * (1.0 - alpha));
}

void saturate(float *signal, float lower_limit, float upper_limit)
{
    if (*signal > upper_limit)
    {
        *signal = upper_limit;
    }
    else if (*signal < lower_limit)
    {
        *signal = lower_limit;
    }
}

float derivative_saturation(float *command, float *prev_command, float max_accel_step, float max_decel_step, float max_command)
{
    float constrained_command = *command;
    saturate(&constrained_command, *prev_command - max_decel_step, *prev_command + max_accel_step);
    saturate(&constrained_command, 0.0, max_command);

    return constrained_command;
}
