#ifndef LTC2943_H_ 
#define LTC2943_H_

#include <stdint.h>

void ltc2943_init(void);

float ltc2943_read_voltage(void);
float ltc2943_read_current(void);
float ltc2943_read_temperature(void);

float ltc2943_read_charge(void);
void ltc2943_set_charge(float charge);

uint8_t ltc2943_read_status(void);

#endif