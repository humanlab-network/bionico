#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "motor.h"

#define THRESHOLD 0.2

int main(void)
{
    int8_t pwm = 30;
    float i = 0;

    stdio_init_all();

    motor_init();

    while(1)
    {
        motor_set_command(pwm);
        sleep_ms(100);

        do
        {
            i = motor_read_current();
            sleep_ms(10);
        } while(fabs(i) < THRESHOLD);

        pwm = -pwm;
    }

    return 0;
}