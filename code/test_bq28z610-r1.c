#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "bq28z610-r1.h"

void print_buffer(uint8_t *data, uint8_t len)
{
    for(uint8_t i = 0; i < len; i++)
    {
        uint8_t d = data[i];
        printf("%02X(%c) ", d, d >= 32 ? (d <= 127 ? d : ' ') : ' ' );
    }
    printf("\n");
}

void convert_date(uint16_t n, uint8_t *d, uint8_t *m, uint16_t *y)
{
    *y = 1980 + (n >> 9);
    *m = 1 + ((n & 0x1FF) >> 5);
    *d = 1 + (n & 0x1F);
}

int main(void)
{
    uint8_t data[256];
    uint8_t d, m;
    uint16_t y;

    stdio_init_all();

    i2c_init(i2c_default, 100000); // 100kHz

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    
    // Wait for serial console to be connected
    while(!stdio_usb_connected());

    bq28z610_r1_init();

    bq28z610_r1_control_read(0x004C, data, 23); data[23] = 0;
    printf("# Manufacturer name: %s\n", &data[2]);

    bq28z610_r1_control_read(0x004A, data, 23); data[23] = 0;
    printf("# Device name: %s\n", &data[2]);
    
    bq28z610_r1_control_read(0x004B, data, 7); data[7] = 0;
    printf("# Device chemistry: %s\n", &data[2]);
    
    bq28z610_r1_control_read(0x004D, data, 4);
    convert_date(*((uint16_t*)&data[2]), &d, &m, &y);
    printf("# Manufacture date: %02u/%02u/%04u\n", d, m, y);

    printf("# Control register: 0x%04X\n", bq28z610_r1_read_register(0x00));
    
    printf("Temperature,Voltage,Current,Remaining capacity,Status\n");

    while(1)
    {
        printf("%f,%f,%f,%f,0x%04X\n",
            bq28z610_r1_get_temperature(),
            bq28z610_r1_get_voltage(),
            bq28z610_r1_get_current(),
            bq28z610_r1_get_remaining_capacity(),
            bq28z610_r1_get_status());
        sleep_ms(1000);
    }

    return 0;
}