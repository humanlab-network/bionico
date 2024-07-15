#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "motor.h"
#include "led.h"

#define THRESHOLD 0.8

int main(void)
{
    int8_t pwm = MAX_PWM;
    float i = 0;

    stdio_init_all();

    motor_init();
    led_init();

    while(1)
    {
        led_set_state(pwm > 0 ? LED1_ON : LED2_ON);

        motor_set_command(pwm);
        sleep_ms(100);

        do
        {
            i = motor_read_current();
            sleep_ms(10);
            printf("%f\n", i);
        } while(fabs(i) < THRESHOLD);

        pwm = -pwm;
    }

    return 0;
}