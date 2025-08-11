#ifndef __HW_MODEL_H__
#define __HW_MODEL_H__
#include <stdint.h>

int ishw(uint16_t);
void hw_write(uint16_t, uint8_t);
uint8_t hw_read(uint16_t);
void hw_init(uint8_t*);
void hw_halt(uint8_t*);

#endif