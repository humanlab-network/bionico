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
#ifndef _BIONICO_H
#define _BIONICO_H

// --- MOTOR ---
#define IN1_PIN 21
#define IN2_PIN 20
#define nSLEEP_PIN 19
#define nFAULT_PIN 18

// --- BATTERY CHARGER ---
#define AC_OK_PIN 24
#define STAT_PIN 25

// --- ADC ---
#define VREF 3.3 // Volt
#define CM_PIN 27
#define CM_CHANNEL 1
#define EL1_PIN 28
#define EL1_CHANNEL 2
#define EL2_PIN 29
#define EL2_CHANNEL 3

// --- LEDs ---
// Mapping:
// LED1 -> GPIO0
// LED2 -> GPIO1
#define LED_PIN(i) (i)

// --- VIBRATOR ---
#define VIBRATOR_PIN 8

// --- EMG INVERTER ---
#define EMG_INVERTER_PIN 11

// --- I2C ---
#define PICO_DEFAULT_I2C 0
#define PICO_DEFAULT_I2C_SDA_PIN 12
#define PICO_DEFAULT_I2C_SCL_PIN 13

// --- FLASH ---
#define PICO_BOOT_STAGE2_CHOOSE_ISL25LP80 1

#endif