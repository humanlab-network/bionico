#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
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
#define TEMP_SECURITY_ACTIVATED
#define VOLTAGE_SECURITY_ACTIVATED

// Measurements
float emg_open_raw, emg_open_filtered = 0.0;
float emg_close_raw, emg_close_filtered = 0.0;
float current_raw, current_filtered = 0.0;
float battery_voltage_raw, battery_voltage_lowfreq_filtered, battery_voltage_highfreq_filtered = 0.0;
float current_ratio, current_ratio_filtered = 0.0;

// Commandenum
#define DIR_OPEN 1
#define DIR_CLOSE -1
float command, prev_command = 0.0;
uint8_t direction, prev_direction = DIR_OPEN;
float motor_command = 0.0;

// Motor control
enum
{
    STATE_RUNNING = 0,
    STATE_STALL_VALIDATION,
    STATE_SHORT_CUTOUT,
    STATE_LONG_CUTOUT
};
uint8_t state = STATE_RUNNING;
uint16_t stall_timer = 0;
uint16_t stall_validation_counter = 0;

float last_stall_command = 0.0;

// Temperature monitoring
float rotor_temp, prev_rotor_temp = T_EXT;
float stator_temp, prev_stator_temp = T_EXT;

// Sampling time handling
uint32_t previous_time = 0;

// Subfunctions declaration
void motor_state_machine(void);
void command_from_emg(void);

int main(void)
{
    int8_t pwm;

    stdio_init_all();

    // Init hardware components
    motor_init();
    electrodes_init();

    // Init filters
    electrodes_read(&emg_open_filtered, &emg_close_filtered);
    current_filtered = motor_read_current();
    battery_voltage_raw = bq28z610_r1_get_voltage();

    // Get starting time
    previous_time = time_us_32();

    while (1)
    {
        // ----- ACQUISITION -----
        // AnalogRead function may take some times !
        electrodes_read(&emg_open_raw, &emg_close_raw);
        current_raw = motor_read_current();
        battery_voltage_raw = bq28z610_r1_get_voltage();

        // ----- MEASURES FILTERING -----
        first_order_filter(&emg_open_filtered, &emg_open_raw, EMG_ALPHA_FILTER);
        first_order_filter(&emg_close_filtered, &emg_close_raw, EMG_ALPHA_FILTER);
        first_order_filter(&current_filtered, &current_raw, CURRENT_ALPHA_FILTER);
        first_order_filter(&battery_voltage_lowfreq_filtered, &battery_voltage_raw, VOLTAGE_ALPHA_LOWFREQ_FILTER);
        first_order_filter(&battery_voltage_highfreq_filtered, &battery_voltage_raw, VOLTAGE_ALPHA_HIGHFREQ_FILTER);

        // ----- GENERATE COMMAND FROM EMG -----
        prev_command = command;
        prev_direction = direction;
        command_from_emg();

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
            current_ratio = R_MOT * current_raw / (battery_voltage_highfreq_filtered * motor_command / MAX_CMD);
        }
        first_order_filter(&current_ratio_filtered, &current_ratio, CURRENT_RATIO_ALPHA_FILTER);

        motor_state_machine();

        motor_command = derivative_saturation(&command, &motor_command, CMD_MAX_ACCEL_STEP, CMD_MAX_DECEL_STEP, MAX_CMD);
#endif

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

        // ----- MOTOR CONTROL ------
        int8_t pwm = (int8_t)((motor_command / MAX_CMD) * MAX_PWM) * direction;
        motor_set_command(pwm);

        // ----- LOOP TIME MONITOR ------
        uint32_t execution_time = (uint32_t)(time_us_32() - previous_time);
        while ((uint32_t)(time_us_32() - previous_time) < PERIOD_US)
        {
        };
        previous_time = time_us_32();
    }

    return 0;
}

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

void motor_state_machine(void)
{
    // State transitions and init
    switch (state)
    {
    case STATE_RUNNING:
        if ((current_ratio_filtered >= STALL_RATIO_THOLD &&
             current_filtered > SAFE_CURRENT))
        {
            state = STATE_STALL_VALIDATION;
            // STALL VALIDATION ENTER
            stall_validation_counter = 0; // Init stall_validation counter
        }
        break;

    case STATE_STALL_VALIDATION:
        if ((current_ratio_filtered < STALL_RATIO_THOLD &&
             stall_validation_counter < STALL_DETECTION_TIME))
        {
            state = STATE_RUNNING;
            // RUNNING ENTER
            // nothing to be done
        }
        else if ((stall_validation_counter >= STALL_DETECTION_TIME &&
                  motor_command * current_ratio_filtered >= 0.95 * command))
        {
            state = STATE_SHORT_CUTOUT;
            // SHORT CUTOUT ENTER

            stall_timer = 0;                    // Init cutout counter
            last_stall_command = motor_command; // Init cmd_stall, TO CHECK
        }
        break;

    case STATE_SHORT_CUTOUT:
        if (stall_timer >= STALL_TIMER_SHORT)
        {
            state = STATE_LONG_CUTOUT;
            // LONG CUTOUT ENTER
            // nothing to be done
        }
        break;

    case STATE_LONG_CUTOUT:
        if ((stall_timer >= STALL_TIMER_LONG || direction != prev_direction ||
             command > STALL_CMD_MARGIN * last_stall_command))
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

    case STATE_STALL_VALIDATION:
        // STALL VALIDATION UPDATE
        stall_validation_counter++; // Increment stall_validation counter
        break;

    case STATE_SHORT_CUTOUT:
        // SHORT CUTOUT UPDATE
        command = 0.0;
        stall_timer++; // Increment cutout counter
        break;

    case STATE_LONG_CUTOUT:
        // LONG CUTOUT UPDATE
        command = 0.0;
        stall_timer++; // Increment cutout counter
        break;
    }
}