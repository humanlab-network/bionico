#include <stdio.h>
#include "pico/stdlib.h"
#include "led.h"

int main(void)
{
    stdio_init_all();

    led_init();

    while(true)
    {
        printf("None\n");
        led_set_state(LEDS_OFF);
        sleep_ms(1000);
        printf("LED1\n");
        led_set_state(LED1_ON);
        sleep_ms(1000);
        printf("LED2\n");
        led_set_state(LED2_ON);
        sleep_ms(1000);
    }

    return 0;
}
