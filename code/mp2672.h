#ifndef MP2672_H_
#define MP2672_H_

void mp2672_init(void);

uint8_t mp2672_get_fault(void);
uint8_t mp2672_get_status(void);

void mp2672_enable_charge(bool enable);

void mp2672_get_regs(uint8_t *reg);

bool mp2672_is_ac_ok(void);

#endif