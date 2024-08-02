#include <stdlib.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/adc.h>
#include "motor.h"

#define R      1200 // Ohm
#define APROPI  450 // µA/A

void motor_init()
{
    // PWM configuration
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 40);    // PWM clock is 133MHz divided by 40, i.e. 3.325MHz
    pwm_config_set_wrap(&config, MAX_PWM); // Wrap is set to 100, meaning the PWM frequency is 33.25kHz

    // Direction pin configuration
    gpio_init(nSLEEP_PIN);
    gpio_init(nFAULT_PIN);
    gpio_set_dir(nSLEEP_PIN, GPIO_OUT);
    gpio_set_dir(nFAULT_PIN, GPIO_IN);

    // Enable motor
    gpio_put(nSLEEP_PIN, 1);

    // PWM pin configuration
    gpio_set_function(IN1_PIN, GPIO_FUNC_PWM);
    gpio_set_function(IN2_PIN, GPIO_FUNC_PWM);
    pwm_init(pwm_gpio_to_slice_num(IN1_PIN), &config, true);
    pwm_init(pwm_gpio_to_slice_num(IN2_PIN), &config, true);

    // ADC configuration
    adc_init();
    adc_gpio_init(CM_PIN);
}

void motor_set_command(int8_t pwm)
{
    bool dir = (pwm > 0);

    // Saturate pwm
    pwm = abs(pwm);
    if (pwm > MAX_PWM)
        pwm = MAX_PWM;

    // Set PWM pins
    pwm_set_gpio_level(IN1_PIN, dir ? pwm : 0);
    pwm_set_gpio_level(IN2_PIN, dir ? 0 : pwm);
}

float motor_read_current(void)
{
    float curr;

    // Select first branch and read current value
    adc_select_input(CM_CHANNEL);
    curr = adc_read();

    // Convert ADC value to volts
    curr *= VREF / (1 << 12);

    // Convert voltage to current
    curr *= 1e6 / (APROPI * R);

    return curr;
}