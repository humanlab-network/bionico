#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "electrodes.h"
#include "motor.h"
#include "signal.h"
#include "control_config.h"
#include "thermal_model.h"
#include "bq28z610-r1.h"

#define LOG_LEVEL LOG_DEBUG
#define NO_DEBUG_HEADER
#include "debug.h"

#define CURRENT_MONITORING_ACTIVATED
// #define TEMP_SECURITY_ACTIVATED
#define VOLTAGE_SECURITY_ACTIVATED
// #define USE_VIRTUAL_MOTOR_RESISTANCE

#define LOG_TO_USB
// #define WAIT_FOR_USB

// Measurements
float emg_open_raw, emg_open_filtered = 0.0;
float emg_close_raw, emg_close_filtered = 0.0;
float current_raw, current_filtered = 0.0;
float battery_voltage_raw, battery_voltage_lowfreq_filtered, battery_voltage_highfreq_filtered = 0.0;
float current_ratio, current_ratio_filtered = 0.0;

// Command
#define DIR_OPEN -1
#define DIR_CLOSE 1
float command, prev_command = 0.0;
float cmd_emg_derivative_filtered = 0.0;
uint8_t direction, prev_direction = DIR_OPEN;
float motor_command = 0.0;

// Motor control
enum
{
    STATE_RUNNING = 0,
    STATE_CUTOUT,
};
uint8_t state = STATE_RUNNING;
uint16_t stall_timer = 0;
uint16_t stall_validation_counter = 0;
float last_stall_command = 0.0;

// Temperature monitoring
float rotor_temp = T_EXT;
float prev_rotor_temp = T_EXT;
float stator_temp = T_EXT;
float prev_stator_temp = T_EXT;

// Sampling time handling
uint32_t previous_time = 0;
uint32_t plot = 0;

/***************** EXTERNAL FUNCTIONS *****************/

// ----- GENERATE PROPORTIONNAL COMMAND AND DIRECTION FROM TWO EMG SIGNALS ------
void command_from_emg(void)
{
    float emg_open_cmd = 0.0;
    float emg_close_cmd = 0.0;

    // If the 2 EMG signals are under MIN Threshold, stop the motor
    if (emg_close_filtered < EMG_CLOSE_MIN_VALUE && emg_open_filtered < EMG_OPEN_MIN_VALUE)
    {
        command = 0.0;
    }
    else
    {
        // Detect which EMG signal is stronger than the other, and constrain its boundaries
        if (emg_close_filtered >= EMG_CLOSE_MIN_VALUE)
        {
            emg_close_cmd = emg_close_filtered;
            saturate(&emg_close_cmd, EMG_CLOSE_MIN_VALUE, EMG_CLOSE_MAX_VALUE);
            // remap EMG signal linearly from [EMG_MIN; EMG_MAX] to [0; SPEED_LIMIT_FLOAT]
            emg_close_cmd = (emg_close_cmd - EMG_CLOSE_MIN_VALUE) * (SPEED_LIMIT_FLOAT / (EMG_CLOSE_MAX_VALUE - EMG_CLOSE_MIN_VALUE));
        } // else 0
        if (emg_open_filtered >= EMG_OPEN_MIN_VALUE)
        {
            emg_open_cmd = emg_open_filtered;
            saturate(&emg_open_cmd, EMG_OPEN_MIN_VALUE, EMG_OPEN_MAX_VALUE);
            // remap EMG signal linearly from [EMG_MIN; EMG_MAX] to [0; SPEED_LIMIT_FLOAT]
            emg_open_cmd = (emg_open_cmd - EMG_OPEN_MIN_VALUE) * (SPEED_LIMIT_FLOAT / (EMG_OPEN_MAX_VALUE - EMG_OPEN_MIN_VALUE));
        } // else 0

        if (emg_close_filtered > emg_open_filtered)
        {
            command = emg_close_cmd;
            direction = DIR_CLOSE;
        }
        else if (emg_open_filtered >= emg_close_filtered)
        {
            command = emg_open_cmd;
            direction = DIR_OPEN;
        }

#ifdef SQUARED_SHAPE
        // apply a "square shape", keeping same boundaries, to be more precise with
        // low EMG signals
        command = (1.0 / SPEED_LIMIT_FLOAT) * command * command;
#endif
#ifdef SQRT_SHAPE
        // apply a "square shape", keeping same boundaries, to be more precise with
        // low EMG signals
        command = sqrt(1.0 * SPEED_LIMIT_FLOAT) * sqrt(command);
#endif
    }
}

// ----- SWITCH ON OR OFF RED LEDS ------
void set_red_leds(bool on)
{
    if (on)
    {
        pwm_set_gpio_level(LED_PIN(0), LED_ON_PWM_RED);
        pwm_set_gpio_level(LED_PIN(2), LED_ON_PWM_RED);
        pwm_set_gpio_level(LED_PIN(4), LED_ON_PWM_RED);
        pwm_set_gpio_level(LED_PIN(6), LED_ON_PWM_RED);
    }
    else
    {
        pwm_set_gpio_level(LED_PIN(0), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(2), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(4), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(6), LED_OFF_PWM);
    }
}

// ----- BRIGHT DIFFERENT LEDS DEPENDING ON BATTERY LEVEL ------
void display_battery_level(float level)
{
    if (level > 0.75)
    {
        pwm_set_gpio_level(LED_PIN(1), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(3), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(5), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(7), LED_ON_PWM_GREEN);
    }
    else if (level > 0.5)
    {
        pwm_set_gpio_level(LED_PIN(1), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(3), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(5), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(7), LED_ON_PWM_GREEN);
    }
    else if (level > 0.25)
    {
        pwm_set_gpio_level(LED_PIN(1), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(3), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(5), LED_ON_PWM_GREEN);
        pwm_set_gpio_level(LED_PIN(7), LED_ON_PWM_GREEN);
    }
    else
    {
        pwm_set_gpio_level(LED_PIN(1), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(3), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(5), LED_OFF_PWM);
        pwm_set_gpio_level(LED_PIN(7), LED_ON_PWM_GREEN);
    }
}

// ----- STATE MACHINE THAT ENABLE MOTOR CUTOUT TO LIMIT HEAT INCREASE ------
/*

void motor_state_machine(void)
{
    // State transitions and init
    switch (state)
    {
    case STATE_RUNNING:
        if (current_ratio_filtered >= STALL_RATIO_THOLD &&
            current_filtered > SAFE_CURRENT &&
            cmd_emg_derivative_filtered <= EMG_DERIVATIVE_THRESHOLD &&
            command - motor_command < CMD_MAX_ACCEL_STEP / 2)
        {
            state = STATE_CUTOUT;
            // CUTOUT ENTER
            stall_validation_counter = 0;       // Init stall_validation counter
            last_stall_command = motor_command; // Save motor command when cutout
            set_red_leds(true);
        }
        break;

    case STATE_CUTOUT:
        if (stall_timer >= STALL_TIMER_LONG ||
            direction != prev_direction ||
            command > STALL_CMD_MARGIN * last_stall_command)
        {
            state = STATE_RUNNING;
            // RUNNING ENTER
            set_red_leds(false);
        }
        break;
    }

    // State update
    switch (state)
    {
    case STATE_RUNNING:
        // RUNNING UPDATE
        // nothing to be done
        break;

    case STATE_CUTOUT:
        // SHORT CUTOUT UPDATE
        command = 0.0;
        stall_timer++; // Increment cutout counter
        break;
    }
}

*/

void motor_state_machine(void)
{
    // State transitions and init
    switch (state)
    {
    case STATE_RUNNING:
        if (current_filtered > 2.0 || current_filtered < -2.0)
        {
            state = STATE_CUTOUT;
            // CUTOUT ENTER
            stall_validation_counter = 0; // Init stall_validation counter
        }
        break;

    case STATE_CUTOUT:
        if (stall_timer >= STALL_TIMER_LONG ||
            direction != prev_direction)
        {
            state = STATE_RUNNING;
            // RUNNING ENTER
            // nothing to be done
        }
        break;
    }

    // State update
    switch (state)
    {
    case STATE_RUNNING:
        // RUNNING UPDATE
        // nothing to be done
        break;

    case STATE_CUTOUT:
        // SHORT CUTOUT UPDATE
        command = 0.0;
        stall_timer++; // Increment cutout counter
        break;
    }
}

int main(void)
{
    /***************** INIT *****************/

    // Init stdio
    stdio_init_all();
    // Init I2C for BMS
    i2c_init(i2c_default, 100000); // 100kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

#ifdef WAIT_FOR_USB
    while (!stdio_usb_connected())
    {
        sleep_ms(10);
    }
    printf("Init starting !");
#endif

    // Init hardware components
    motor_init();
    electrodes_init();
    bq28z610_r1_init();

    // Init filters
    electrodes_read(&emg_open_filtered, &emg_close_filtered);
    current_filtered = motor_read_current();
    battery_voltage_raw = bq28z610_r1_get_voltage();
    battery_voltage_highfreq_filtered = battery_voltage_raw;
    battery_voltage_lowfreq_filtered = battery_voltage_raw;

    // Get starting time
    previous_time = time_us_32();

    /***************** LOOP *****************/
    while (1)
    {
        // ----- ACQUISITION -----
        // AnalogRead function may take some times !
        electrodes_read(&emg_open_raw, &emg_close_raw);
        current_raw = motor_read_current();
        battery_voltage_raw = bq28z610_r1_get_voltage();

        // ----- MEASURES FILTERING -----
        emg_open_filtered = first_order_filter(&emg_open_filtered, &emg_open_raw, EMG_ALPHA_FILTER);
        emg_close_filtered = first_order_filter(&emg_close_filtered, &emg_close_raw, EMG_ALPHA_FILTER);
        current_filtered = first_order_filter(&current_filtered, &current_raw, CURRENT_ALPHA_FILTER);
        battery_voltage_lowfreq_filtered = first_order_filter(&battery_voltage_lowfreq_filtered, &battery_voltage_raw, VOLTAGE_ALPHA_LOWFREQ_FILTER);
        battery_voltage_highfreq_filtered = first_order_filter(&battery_voltage_highfreq_filtered, &battery_voltage_raw, VOLTAGE_ALPHA_HIGHFREQ_FILTER);

        // ----- GENERATE COMMAND FROM EMG -----
        prev_command = command;
        prev_direction = direction;
        command_from_emg();
        cmd_emg_derivative_filtered = first_order_filter(&cmd_emg_derivative_filtered, &command, CMD_EMG_ALPHA_FILTER);

        // ----- TEMPERATURE ESTIMATION ------
        prev_rotor_temp = rotor_temp;
        prev_stator_temp = stator_temp;
        run_thermal_model(&rotor_temp, &prev_rotor_temp, &stator_temp, &prev_stator_temp, &current_filtered);

        // ----- STALL TORQUE MONITORING ------
#ifdef CURRENT_MONITORING_ACTIVATED
        // ratio ~ R * Imot / Umot; when ratio~=1, the motor is stalled
        // Avoiding calculating ratio for small commands to avoid division by 0 (or
        // noise) Division by 8 is supposed to be fast !
        if (motor_command > MAX_CMD / 8 && battery_voltage_highfreq_filtered > 0)
        {
#ifdef USE_VIRTUAL_MOTOR_RESISTANCE
            current_ratio = VIRTUAL_R_MOT * current_raw / (battery_voltage_highfreq_filtered * motor_command / MAX_CMD);
#else
            current_ratio = R_MOT * current_raw / (battery_voltage_highfreq_filtered * motor_command / MAX_CMD);
#endif
        }
        current_ratio_filtered = first_order_filter(&current_ratio_filtered, &current_ratio, CURRENT_RATIO_ALPHA_FILTER);

        motor_state_machine();
#endif
        motor_command = derivative_saturation(&command, &motor_command, CMD_MAX_ACCEL_STEP, CMD_MAX_DECEL_STEP, MAX_CMD);

        // ----- TEMPERATURE SECURITY CHECK ------
#ifdef TEMP_SECURITY_ACTIVATED
        if (rotor_temp - stator_temp > DELTA_TEMP_LIMIT || stator_temp > STATOR_TEMP_LIMIT || rotor_temp > ROTOR_TEMP_LIMIT)
        {
            motor_command = 0.0;
        }
#endif

        // ----- VOLTAGE SECURITY CHECK ------
#ifdef VOLTAGE_SECURITY_ACTIVATED
        if (battery_voltage_lowfreq_filtered < MIN_VOLTAGE)
        {
            motor_command = 0.0;
        }
#endif

        // ----- USING VIRTUAL RESISTANCE TO LOWER STALL CURRENT -----
#ifdef USE_VIRTUAL_MOTOR_RESISTANCE
        float motor_command_corrected = motor_command - (VIRTUAL_R_MOT - R_MOT) * current_filtered / battery_voltage_highfreq_filtered * MAX_CMD;
        int8_t pwm = (int8_t)((motor_command_corrected / MAX_CMD) * MAX_PWM) * direction;
#else
        int8_t pwm = (int8_t)((motor_command / MAX_CMD) * MAX_PWM) * direction;
#endif

        // ----- MOTOR PWM WRITE ------
        motor_set_command(pwm);

        // ----- BATTERY LEVEL DISPLAY -----
        float battery_level = bq28z610_r1_get_remaining_capacity() / BATTERY_FULL_CAPACITY;
        display_battery_level(battery_level);

        // ----- LOG DATA TO USB ------
#ifdef LOG_TO_USB
        if (stdio_usb_connected() && plot >= 20)
        {
            printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%i,%.2f,%.2f\n",
                   emg_close_filtered,
                   emg_open_filtered,
                   current_filtered,
                   current_ratio_filtered,
                   battery_voltage_lowfreq_filtered,
                   battery_voltage_highfreq_filtered,
                   command,
                   motor_command,
                   pwm,
                   rotor_temp,
                   stator_temp);
            plot = 0;
        }
        plot++;
#endif

        // ----- LOOP TIME MONITOR ------

        uint32_t execution_time = (uint32_t)(time_us_32() - previous_time);
        while ((uint32_t)(time_us_32() - previous_time) < PERIOD_US)
        {
        };
        previous_time = time_us_32();
    }

    return 0;
}
