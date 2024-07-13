#ifndef _LED_H
#define _LED_H

typedef enum
{
    LEDS_OFF,
    LED1_ON,
    LED2_ON
} led_state_t;

void led_init(void);

void led_set_state(led_state_t state);

#endif