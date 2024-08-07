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