#ifndef _PLATFORM_H
#define _PLATFORM_H

// --- LEDs ---
// Mapping:
// LED0 -> GPIO0
// LED1 -> GPIO1
// LED2 -> GPIO2
// LED3 -> GPIO3
// LED4 -> GPIO8
// LED5 -> GPIO9
// LED6 -> GPIO10
// LED7 -> GPIO11
#define LED_PIN(i) (i <= 3 ? i : 8 + i - 4)

// --- I2C ---
#define PICO_DEFAULT_I2C           0
#define PICO_DEFAULT_I2C_SDA_PIN   12
#define PICO_DEFAULT_I2C_SCL_PIN   13

// --- FLASH ---
#define PICO_BOOT_STAGE2_CHOOSE_ISL25LP80 1

#endif