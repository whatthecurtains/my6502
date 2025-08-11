
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define LSR(mode,cc) \
all_regs_t* LSR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    reg->SR.C = OP&1;                                                               \
    OP >>=1;                                                                        \
    reg->A = OP;                                                                    \
    reg->SR.N = 0;                                                                  \
    reg->SR.Z = (reg->A==0) ? 1 : 0;                                                \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "LSR_def.h"
