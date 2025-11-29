

#include <stdint.h>
#include "keydecode.h"

uint8_t OSI_keystate[8];

static inline uint8_t keycode_down(uint8_t keycode, uint8_t key) {
    return keycode | (1<<key);
}

static inline uint8_t keycode_up(uint8_t keycode, uint8_t key) {
    return  keycode & ~(1<<key);
}

void update_keystate(bool down, uint16_t val, uint8_t key) {
    uint32_t index=key<<16|val;
    uint8_t  row;
    uint8_t  col;
    switch(index) {
    default: goto skip;    
    case 0xa0031: row=7; col=7; break;
    case 0xb0032: row=7; col=6; break;
    case 0xc0033: row=7; col=5; break;
    case 0xd0034: row=7; col=4; break;
    case 0xe0035: row=7; col=3; break;
    case 0xf0036: row=7; col=2; break;
    case 0x100037: row=7; col=1; break;
    case 0x110038: row=6; col=7; break;
    case 0x120039: row=6; col=6; break;
    case 0x130030: row=6; col=5; break;
    case 0x14002d: row=6; col=4; break;
    case 0x15003d: row=6; col=3; break;
    case 0x16ff08: row=6; col=2; break;
    case 0x3c002e: row=5; col=7; break;
    case 0x2e006c: row=5; col=6; break;
    case 0x20006f: row=5; col=5; break;
    case 0x74ff54: row=5; col=4; break;
    case 0x24ff0d: row=5; col=3; break;
    case 0x190077: row=4; col=7; break;
    case 0x1a0065: row=4; col=6; break;
    case 0x1b0072: row=4; col=5; break;
    case 0x1c0074: row=4; col=4; break;
    case 0x1d0079: row=4; col=3; break;
    case 0x1e0075: row=4; col=2; break;
    case 0x1f0069: row=4; col=1; break;
    case 0x270073: row=3; col=7; break;
    case 0x280064: row=3; col=6; break;
    case 0x290066: row=3; col=5; break;
    case 0x2a0067: row=3; col=4; break;
    case 0x2b0068: row=3; col=3; break;
    case 0x2c006a: row=3; col=2; break;
    case 0x2d006b: row=3; col=1; break;
    case 0x350078: row=2; col=7; break;
    case 0x360063: row=2; col=6; break;
    case 0x370076: row=2; col=5; break;
    case 0x380062: row=2; col=4; break;
    case 0x39006e: row=2; col=3; break;
    case 0x3a006d: row=2; col=2; break;
    case 0x3b002c: row=2; col=1; break;
    case 0x180071: row=1; col=7; break;
    case 0x260061: row=1; col=6; break;
    case 0x34007a: row=1; col=5; break;
    case 0x410020: row=1; col=4; break;
    case 0x3d002f: row=1; col=3; break;
    case 0x2f003b: row=1; col=2; break;
    case 0x210070: row=1; col=1; break;
    case 0x77ffff: row=0; col=7; break;
    case 0x25ffe3: row=0; col=6; break;
    case 0x9ff1b: row=0; col=5; break;
    case 0x32ffe1: row=0; col=2; break;
    case 0x3effe2: row=0; col=1; break;
    case 0x42ffe5: row=0; col=0; break;
    }

    if (down) {
        OSI_keystate[row] = keycode_down(OSI_keystate[row],col);
    }
    else {
        OSI_keystate[row] = keycode_up(OSI_keystate[row],col);
    }
skip:
    return;
}

