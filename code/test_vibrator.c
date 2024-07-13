#include <stdio.h>
#include "pico/stdlib.h"
#include "vibrator.h"

//static const uint16_t sin[] = {0, 309, 587, 809, 951, 1000, 951, 809, 587, 309};
static const uint16_t sin[] = {0, 156, 309, 453, 587, 707, 809, 891, 951, 987, 1000, 987, 951, 891, 809, 707, 587, 453, 309, 156};
#define N (sizeof(sin) / sizeof(sin[0]))

int main(void)
{
    uint16_t t = 0;

    stdio_init_all();

    vibrator_init();

    while(true)
    {
        vibrator_set_value(sin[t % N]);
        sleep_ms(100);
        t++;
    }

    return 0;
}
