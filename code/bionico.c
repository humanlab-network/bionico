/*
 * Bionico code is the firmware source code embedded in the electronic board
 * dedicated to the prothestic hand [bionicohand](https://bionico.org/)
 * Copyright 2024 INRIA
 * Contributors: see AUTHORS file
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "ltc2943.h"
#include "mp2672.h"
#include "vibrator.h"
#include "electrodes.h"
#include "motor.h"
#include "led.h"
#include "signal.h"
#include "thermal_model.h"
#include "command.h"
#include "control_config.h"

#define LED_GREEN LED1_ON
#define LED_RED LED2_ON

// #define LOG_TO_USB
// #define WAIT_FOR_USB
// #define DEACTIVATE_VIBRATOR

// Measurements
float emg_open_raw, emg_open_filtered;
float emg_close_raw, emg_close_filtered;
float current_raw, current_filtered;
float battery_voltage_raw, battery_voltage_lowfreq_filtered, battery_voltage_highfreq_filtered;
float current_ratio, current_ratio_filtered;

// Command
#define DIR_OPEN -1
#define DIR_CLOSE 1
float command, prev_command = 0.0;
float cmd_emg_derivative_filtered = 0.0;
int8_t direction, prev_direction = DIR_OPEN;
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
absolute_time_t previous_time;
uint32_t plot = 0;

// ----- STATE MACHINE THAT ENABLE MOTOR CUTOUT TO LIMIT HEAT INCREASE ------
void motor_state_machine(void)
{
    // State transitions and init
    switch (state)
    {
    case STATE_RUNNING:
        /*if (current_ratio_filtered >= STALL_RATIO_THOLD &&
            current_filtered > SAFE_CURRENT &&
            cmd_emg_derivative_filtered <= EMG_DERIVATIVE_THRESHOLD &&
            command - motor_command < CMD_MAX_ACCEL_STEP / 2)*/
        if (fabsf(current_filtered) > SAFE_CURRENT)
        {
            state = STATE_CUTOUT;
            // CUTOUT ENTER
            stall_validation_counter = 0; // Init stall_validation counter
            /* last_stall_command = motor_command; // Save motor command when cutout */
        }
        break;

    case STATE_CUTOUT:
        if (stall_timer >= STALL_TIMER_LONG ||
            direction != prev_direction) /* command > STALL_CMD_MARGIN * last_stall_command) */
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

void init(void)
{
    // Wait before start in case power is not completely stable
    sleep_ms(100);

    // Init stdio
    stdio_init_all();

    // Init I2C for BMS
    i2c_init(i2c_default, 100000); // 100kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Init hardware components
    led_init();

    motor_init();
    electrodes_init();
    ltc2943_init();
    vibrator_init();
    mp2672_init();

    gpio_init(EMG_INVERTER_PIN);
    gpio_set_dir(EMG_INVERTER_PIN, GPIO_IN);
    gpio_pull_up(EMG_INVERTER_PIN);

#ifdef WAIT_FOR_USB
    while (!stdio_usb_connected())
    {
        sleep_ms(10);
    }
#endif
}

void check_charge(void)
{
    float charge = ltc2943_read_charge();

    if (charge > 0.8 * BATTERY_FULL_CAPACITY)
    {
        // Battery is almost fully charged, show green LED for 2s
        led_set_state(LED_GREEN);
        sleep_ms(2000);
        led_set_state(LEDS_OFF);
    }
    else if (charge < 0.2 * BATTERY_FULL_CAPACITY)
    {
        // Battery is almost depleted, show red LED for 2s
        led_set_state(LED_RED);
        sleep_ms(2000);
        led_set_state(LEDS_OFF);
    }
    else
    {
        // Battery is in between, blink green LED for 2s
        for (uint8_t i = 0; i < 4; i++)
        {
            led_set_state(LED_GREEN);
            sleep_ms(250);
            led_set_state(LEDS_OFF);
            sleep_ms(250);
        }
    }
}

void do_acquisition(void)
{
    // ----- ACQUISITION -----
    if (gpio_get(EMG_INVERTER_PIN))
    {
        electrodes_read(&emg_open_raw, &emg_close_raw);
    }
    else
    {
        electrodes_read(&emg_close_raw, &emg_open_raw);
    }
    current_raw = motor_read_current();
    battery_voltage_raw = ltc2943_read_voltage();

    // ----- MEASURES FILTERING -----
    emg_open_filtered = first_order_filter(emg_open_filtered, emg_open_raw, EMG_ALPHA_FILTER);
    emg_close_filtered = first_order_filter(emg_close_filtered, emg_close_raw, EMG_ALPHA_FILTER);
    current_filtered = first_order_filter(current_filtered, current_raw, CURRENT_ALPHA_FILTER);
    battery_voltage_lowfreq_filtered = first_order_filter(battery_voltage_lowfreq_filtered, battery_voltage_raw, VOLTAGE_ALPHA_LOWFREQ_FILTER);
    battery_voltage_highfreq_filtered = first_order_filter(battery_voltage_highfreq_filtered, battery_voltage_raw, VOLTAGE_ALPHA_HIGHFREQ_FILTER);
}

void stall_torque_monitoring(void)
{
    // ratio ~ R * Imot / Umot; when ratio~=1, the motor is stalled
    // Avoiding calculating ratio for small commands to avoid division by 0 (or
    // noise) Division by 8 is supposed to be fast !
    if ((motor_command > MAX_CMD / 8) && (battery_voltage_highfreq_filtered > 0))
    {
#ifdef USE_VIRTUAL_MOTOR_RESISTANCE
        float r = VIRTUAL_R_MOT;
#else
        float r = R_MOT;
#endif
        current_ratio = r * current_raw / (battery_voltage_highfreq_filtered * motor_command / MAX_CMD);
    }
    current_ratio_filtered = first_order_filter(current_ratio_filtered, current_ratio, CURRENT_RATIO_ALPHA_FILTER);

    motor_state_machine();
}

void vibration(float current)
{
#define VIB_THRESHOLD (SAFE_CURRENT * 0.4)

    if (current < VIB_THRESHOLD)
    {
        vibrator_set_value(0);
        return;
    }

#ifndef DEACTIVATE_VIBRATOR
    vibrator_set_value((uint16_t)(1000 * (saturate(current, VIB_THRESHOLD, SAFE_CURRENT) - VIB_THRESHOLD) / (SAFE_CURRENT - VIB_THRESHOLD)));
#else
    vibrator_set_value(0);
#endif
}

void stop_while_charging(void)
{
    static bool blink = true;
    static bool mp2672_is_initialized = false;

    while (mp2672_is_ac_ok())
    {
        // Do not try to initialize the MP2672 charger until AC is OK for the first time, the chip won't answer
        if (!mp2672_is_initialized)
        {
            mp2672_configure();

            // Enable charger
            mp2672_enable_charge(true);
            mp2672_is_initialized = true;
        }

        uint8_t s = (mp2672_get_status() >> 4) & 0x3;

        switch (s)
        {
        case 0:
            // Not charging
            led_set_state(LED_RED);
            break;

        case 1:
        case 2:
            // Charging or pre-charging
            led_set_state(blink ? LED_GREEN : LEDS_OFF);
            blink = !blink;
            break;

        case 3:
            // Fully charged, reset battery gauge
            led_set_state(LED_GREEN);
            ltc2943_set_charge(BATTERY_FULL_CAPACITY);
            break;
        }

        motor_set_command(0);
        sleep_ms(1000);
    }

    led_set_state(LEDS_OFF);
}

int main(void)
{
    // Hardware initialization
    init();

    // Check battery charge and inform user
    check_charge();

    // Filters initialization
    electrodes_read(&emg_open_filtered, &emg_close_filtered);
    current_filtered = motor_read_current();
    battery_voltage_highfreq_filtered = battery_voltage_lowfreq_filtered = ltc2943_read_voltage();

    // Get starting time
    previous_time = get_absolute_time();

    while (true)
    {
#ifndef LOG_TO_USB
        stop_while_charging();
#endif

        do_acquisition();

        // Generate command from EMG
        prev_command = command;
        prev_direction = direction;
        command_from_emg(emg_close_filtered, emg_open_filtered, &command, &direction);
        cmd_emg_derivative_filtered = first_order_filter(cmd_emg_derivative_filtered, command, CMD_EMG_ALPHA_FILTER);

        // Temperature estimation
        prev_rotor_temp = rotor_temp;
        prev_stator_temp = stator_temp;
        run_thermal_model(&rotor_temp, prev_rotor_temp, &stator_temp, prev_stator_temp, current_filtered);

        // Stall torque monitoring
#ifdef CURRENT_MONITORING_ACTIVATED
        stall_torque_monitoring();
#endif
        motor_command = derivative_saturation(command, motor_command, CMD_MAX_ACCEL_STEP, CMD_MAX_DECEL_STEP, MAX_CMD);

        // Temperature security check
#ifdef TEMP_SECURITY_ACTIVATED
        if (rotor_temp - stator_temp > DELTA_TEMP_LIMIT || stator_temp > STATOR_TEMP_LIMIT || rotor_temp > ROTOR_TEMP_LIMIT)
        {
            motor_command = 0.0;
        }
#endif

        // Battery voltage security check
#ifdef VOLTAGE_SECURITY_ACTIVATED
        if (battery_voltage_lowfreq_filtered < MIN_VOLTAGE)
        {
            motor_command = 0.0;
            led_set_state(LED_RED);
        }
        else
        {
            led_set_state(LEDS_OFF);
        }
#endif

        // Using virtual resistance to lower stall current
#ifdef USE_VIRTUAL_MOTOR_RESISTANCE
        float motor_command_corrected = motor_command - (VIRTUAL_R_MOT - R_MOT) * current_filtered / battery_voltage_highfreq_filtered * MAX_CMD;
#else
        float motor_command_corrected = motor_command;
#endif
        int8_t pwm = (int8_t)((motor_command_corrected / MAX_CMD) * MAX_PWM) * direction;

        // Set motor command
        motor_set_command(pwm);

        // Set vibration level for haptic feedback
        vibration(current_raw);

        // Log data to USB
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
        sleep_until(delayed_by_us(previous_time, PERIOD_US));
        previous_time = get_absolute_time();
    }

    return 0;
}
