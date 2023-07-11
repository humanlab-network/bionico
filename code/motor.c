#include <stdlib.h>
#include <hardware/gpio.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "motor.h"

#define MAX_PWM 100
#define R 5100   // Ohm

void motor_init()
{
    // PWM configuration
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 40);    // PWM clock is 133MHz divided by 40, i.e. 3.325MHz
    pwm_config_set_wrap(&config, MAX_PWM); // Wrap is set to 100, meaning the PWM frequency is 33.25kHz

    // Direction pin configuration
    gpio_init(IN1);
    gpio_init(IN2);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);

    // PWM pin configuration
    gpio_set_function(EN1, GPIO_FUNC_PWM);
    gpio_set_function(EN2, GPIO_FUNC_PWM);
    pwm_init(pwm_gpio_to_slice_num(EN1), &config, true);
    pwm_init(pwm_gpio_to_slice_num(EN2), &config, true);

    // ADC configuration
    adc_init();
    adc_gpio_init(CM1_GPIO);
    adc_gpio_init(CM2_GPIO);
}

void motor_set_command(int8_t pwm)
{
    bool dir = (pwm > 0);

    // Saturate pwm
    pwm = abs(pwm);
    if(pwm > MAX_PWM)
        pwm = MAX_PWM;

    // Set direction pins
    gpio_put(IN1, dir ? 1 : 0);
    gpio_put(IN2, dir ? 0 : 1);
    
    // Set PWM pins
    pwm_set_gpio_level(EN1, dir ? pwm : MAX_PWM);
    pwm_set_gpio_level(EN2, dir ? MAX_PWM : pwm);
}

float motor_read_current(void)
{
    float curr_a, curr_b;

    // Select first branch and read current value
    adc_select_input(CM1_CHANNEL);
    curr_a = adc_read();

    // Select second branch and read current value
    adc_select_input(CM2_CHANNEL);
    curr_b = adc_read();

    // Convert ADC value to volts
    curr_a *= VREF / (1 << 12);
    curr_b *= VREF / (1 << 12);

    // Get value relative to middle point
    curr_a -= VREF / 2;
    curr_b -= VREF / 2;

    // Convert voltage to current
    curr_a *= 10000 / R;
    curr_b *= 10000 / R;

    return curr_a - curr_b;
}