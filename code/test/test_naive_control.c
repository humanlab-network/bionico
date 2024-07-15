#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "electrodes.h"
#include "motor.h"

#define LOG_LEVEL LOG_DEBUG
#define NO_DEBUG_HEADER
#include "debug.h"

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
#define THRESHOLD 0.005
#define MAX_SPEED 70     // Between 0 and 100
#define MAX_TORQUE 0.3   // A

enum
{
    STATE_UNDEFINED = 0,
    STATE_CLOSING,
    STATE_CLOSED,
    STATE_OPENING,
    STATE_OPEN
};

uint8_t state = STATE_UNDEFINED;

int main(void)
{
    float signal1 = 0, signal2 = 0, mu1, mu2, sigma1, sigma2, i;
    bool open, close, stop;
    int8_t pwm;

    stdio_init_all();

    motor_init();
    electrodes_init();

    while(1)
    {
        get_electrodes_stat(&mu1, &mu2, &sigma1, &sigma2);
        i = motor_read_current();

        signal1 = ALPHA * signal1 + (1 - ALPHA) * sigma1; // Close signal
        signal2 = ALPHA * signal2 + (1 - ALPHA) * sigma2; // Open signal

        close = signal1 > THRESHOLD;
        open = signal2 > THRESHOLD;
        stop = fabs(i) > MAX_TORQUE;

        switch(state)
        {
            case STATE_UNDEFINED:
                if(close)
                {
                    state = STATE_CLOSING;
                    log_info("Closing\n");
                }
                else if(open)
                {
                    state = STATE_OPENING;
                    log_info("Opening\n");
                }
                else
                {
                    pwm = 0;
                }
                break;

            case STATE_CLOSING:
                if(open)
                {
                    state = STATE_OPENING;
                    pwm = -MAX_SPEED;
                    log_info("Opening\n");
                }
                else if(stop)
                {
                    state = STATE_CLOSED;
                    pwm = 0;
                    log_info("Closed\n");
                }
                else
                {
                    pwm = MAX_SPEED;
                }
                break;

            case STATE_CLOSED:
                if(open)
                {
                    state = STATE_OPENING;
                    log_info("Opening\n");
                }
                else
                {
                    pwm = 0;
                }
                break;

            case STATE_OPENING:
                if(close)
                {
                    state = STATE_CLOSING;
                    pwm = MAX_SPEED;
                    log_info("Closing\n");
                }
                else if(stop)
                {
                    state = STATE_OPEN;
                    log_info("Open\n");
                    pwm = 0;
                }
                else
                {
                    pwm = -MAX_SPEED;
                }
                break;

            case STATE_OPEN:
                if(close)
                {
                    state = STATE_CLOSING;
                    log_info("Closing\n");
                }
                else
                {
                    pwm = 0;
                }
                break;
        }

        motor_set_command(pwm);
    }

    return 0;
}