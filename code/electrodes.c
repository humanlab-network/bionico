#include "hardware/adc.h"
#include "electrodes.h"

void electrodes_init(void)
{
    adc_init();

    adc_gpio_init(EL1_PIN);
    adc_gpio_init(EL2_PIN);
}

void electrodes_read(float *el1, float *el2)
{
    // Read electrode 1 value
    adc_select_input(EL1_CHANNEL);
    *el1 = adc_read() * VREF / (1 << 12);
    
    // Read electrode 2 value
    adc_select_input(EL2_CHANNEL);
    *el2 = adc_read() * VREF / (1 << 12);
}