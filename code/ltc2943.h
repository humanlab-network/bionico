#ifndef LTC2943_H_ 
#define LTC2943_H_

#include <stdint.h>

void ltc2943_init(void);

float ltc2943_read_voltage(void);
float ltc2943_read_current(void);

#endif