#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stdbool.h>

void motor_init();

void motor_set_command(int8_t pwm);

float motor_read_current(void);

#endif