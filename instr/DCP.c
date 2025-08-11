#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define DCP(mode,cc) \
all_regs_t* DCP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    uint8_t dif;                                                                    \
    mode(reg,low,high,1)                                                            \
    OP += 0xFF;                                                                     \
    dif = reg->PC-OP;                                                               \
    reg->SR.Z = (dif==0) ? 1 : 0;                                                   \
    reg->SR.N = ((dif&0x80)==0x80) ? 1 : 0;                                         \
    memwr(addr,OP);                                                                 \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "DCP_def.h"
