
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define INY(mode,cc) \
all_regs_t* INY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    reg->Y += 1;                                                                        \
    reg->SR.Z = (reg->Y==0) ? 1 : 0;                                                    \
    reg->SR.N = ((reg->Y&0x80)==0x80) ? 1 : 0;                                          \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "INY_def.h"
