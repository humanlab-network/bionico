#ifndef BQ28Z610_R1_H_
#define BQ28Z610_R1_H_

void bq28z610_r1_init(void);

float bq28z610_r1_get_voltage(void);
float bq28z610_r1_get_current(void);
float bq28z610_r1_get_temperature(void);
float bq28z610_r1_get_remaining_capacity(void);
uint16_t bq28z610_r1_get_status(void);
uint16_t bq28z610_r1_read_register(uint8_t addr); // TODO: static
uint16_t bq28z610_r1_control_read(uint16_t cmd, uint8_t *data, uint8_t len); // TODO: static

#endif