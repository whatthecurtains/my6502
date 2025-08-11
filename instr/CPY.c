
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define CPY(mode,cc) \
all_regs_t* CPY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    uint16_t cmp = reg->Y - OP;                                                     \
    reg->SR.Z = (cmp==0) ? 1 : 0;                                                   \
    reg->SR.N = ((cmp&0x80) != 0) ? 1 : 0;                                          \
    reg->SR.C = ((cmp&0x8000) == 0) ? 1 : 0;                                        \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "CPY_def.h"
