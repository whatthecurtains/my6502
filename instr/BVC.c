
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define BVC(mode,cc) \
all_regs_t* BVC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,1)                                                                \
    int branch=(reg->SR.V==0) ? 1 : 0;                                                  \
    uint16_t next_pc=(int16_t)reg->PC + 2 + OP;                                         \
    int page= (branch==1) ? (((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0) : 0;         \
    *cyc += 2 + branch + page ;                                                         \
    reg->PC =  (branch==1) ? next_pc : reg->PC;                                         \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "BVC_def.h"
