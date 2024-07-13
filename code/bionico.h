#ifndef _BIONICO_H
#define _BIONICO_H

// --- MOTOR ---
#define IN1_PIN 18
#define IN2_PIN 19
#define nSLEEP_PIN 20
#define nFAULT_PIN 21

// --- ADC ---
#define VREF 3.3  // Volt
#define BAT_PIN 26
#define BAT_CHANNEL 0
#define CM_PIN 27
#define CM_CHANNEL 1
#define EL1_PIN 28 
#define EL1_CHANNEL 2
#define EL2_PIN 29
#define EL2_CHANNEL 3

// --- LEDs ---
// Mapping:
// LED1 -> GPIO2
// LED2 -> GPIO3
#define LED_PIN(i) (i + 2)


// --- VIBRATOR ---
#define VIBRATOR_PIN 8

// --- I2C ---
#define PICO_DEFAULT_I2C           0
#define PICO_DEFAULT_I2C_SDA_PIN   12
#define PICO_DEFAULT_I2C_SCL_PIN   13

// --- FLASH ---
#define PICO_BOOT_STAGE2_CHOOSE_ISL25LP80 1

#endif