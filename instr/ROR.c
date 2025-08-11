
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define ROR(mode,cc) \
all_regs_t* ROR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    reg->A = (OP)>>1 | (reg->SR.C<<7);                                              \
    reg->SR.N = (reg->A&0x80)!=0 ? 1 : 0;                                           \
    reg->SR.Z = (reg->A==0)      ? 1 : 0;                                           \
    reg->SR.C = OP&0x01 ? 1 : 0;                                                    \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "ROR_def.h"
