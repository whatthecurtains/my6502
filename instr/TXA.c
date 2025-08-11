
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"


#define TXA(mode,cc) \
all_regs_t* TXA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    reg->A = reg->X;                                                                    \
    reg->SR.N = (reg->A&0x80)!=0 ? 1 : 0;                                               \
    reg->SR.Z = (reg->A==0)      ? 1 : 0;                                               \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "TXA_def.h"
