#ifndef _SIGNAL_H_
#define _SIGNAL_H_

float first_order_filter(float prev_value, float new_value, float alpha);

float saturate(float signal, float lower_limit, float upper_limit);

float derivative_saturation(float command, float prev_command, float max_accel_step, float max_decel_step, float max_command);

#endif