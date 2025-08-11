
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define LDY(mode,cc) \
all_regs_t* LDY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    reg->Y = OP;                                                                    \
    reg->SR.N = ((reg->Y&0x80)==0x80) ? 1 : 0;                                      \
    reg->SR.Z = (reg->Y==0) ? 1 : 0;                                                \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "LDY_def.h"
