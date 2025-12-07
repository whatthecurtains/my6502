#ifndef __KEYDECODE_H__
#define __KEYDECODE_H__
#include <stdbool.h>

extern uint8_t OSI_keystate[8];

void update_keystate( bool down, uint16_t val, uint8_t key);

#endif //__KEYDECODE_H__