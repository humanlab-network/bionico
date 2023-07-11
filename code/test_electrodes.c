#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "electrodes.h"

#define N 50

void get_electrodes_stat(float *mu1, float *mu2, float *sigma1, float *sigma2)
{
    float e1, e2;

    *mu1 = 0;
    *mu2 = 0;
    *sigma1 = 0;
    *sigma2 = 0;

    for(uint8_t i = 0; i < N; i++)
    {
        electrodes_read(&e1, &e2);
        *mu1 += e1;
        *mu2 += e2;
        *sigma1 += e1 * e1;
        *sigma2 += e2 * e2;
        sleep_ms(1);
    }

    *mu1 /= N;
    *mu2 /= N;
    *sigma1 = sqrtf(*sigma1 / N - *mu1 * *mu1);
    *sigma2 = sqrtf(*sigma2 / N - *mu2 * *mu2);
}

#define ALPHA 0.8

int main(void)
{
    float signal1 = 0, signal2 = 0, mu1, mu2, sigma1, sigma2;

    stdio_init_all();

    electrodes_init();

    while(1)
    {
        get_electrodes_stat(&mu1, &mu2, &sigma1, &sigma2);

        signal1 = ALPHA * signal1 + (1 - ALPHA) * sigma1;
        signal2 = ALPHA * signal2 + (1 - ALPHA) * sigma2;
        
        printf("%f %f\n", signal1, signal2);
    }

    return 0;
}