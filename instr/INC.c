
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define INC(mode,cc) \
all_regs_t* INC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    OP += 1;                                                                        \
    reg->SR.Z = (OP==0) ? 1 : 0;                                                    \
    reg->SR.N = ((OP&0x80)==0x80) ? 1 : 0;                                          \
    memwr(addr,OP);                                                                 \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "INC_def.h"
