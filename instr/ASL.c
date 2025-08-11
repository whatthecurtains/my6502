
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define ASL(mode,cc) \
all_regs_t* ASL_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    reg->SR.C = OP&0x80 ? 1 : 0;                                                    \
    reg->A = (OP)<<1;                                                               \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}


#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)

#include "ASL_def.h"
