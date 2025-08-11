
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define BIT(mode,cc) \
all_regs_t* BIT_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    uint8_t tmp = OP & reg->A;                                                      \
    reg->SR.N = (tmp&80) ? 1 : 0;                                                   \
    reg->SR.V = (tmp&40) ? 1 : 0;                                                   \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "BIT_def.h"
