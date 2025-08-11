#ifndef __REGS_H__
#define __REGS_H__

#include <stdint.h>

typedef struct {
    uint16_t         C:1;
    uint16_t         Z:1;
    uint16_t         I:1;
    uint16_t         D:1;
    uint16_t         B:1;
    uint16_t         _nothing:1;
    uint16_t         V:1;
    uint16_t         N:1;
} status_reg_t;

typedef struct {
    uint16_t        PC;
    uint8_t         A;
    uint8_t         X;
    uint8_t         Y;
    uint8_t         SP;
    union {
        status_reg_t    SR;
        uint8_t         SR_all;
    };
    uint64_t        cyc:64;
    uint8_t         brk;
} all_regs_t;

typedef all_regs_t* (*opcode_fn)(all_regs_t*, uint8_t, uint8_t, uint64_t*);

typedef struct {
    opcode_fn   opcf;
    const char* inst;
    const char* mode;
    uint8_t     hex;
    uint8_t     bytes;
} opcode_table_t;

#endif
