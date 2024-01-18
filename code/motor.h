#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_PWM 100
#define R 5100 // Ohm

void motor_init();

void motor_set_command(int8_t pwm);

float motor_read_current(void);

#endif