
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define DEX(mode,cc) \
all_regs_t* DEX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,0)                                                            \
    OP = reg->X;                                                                    \
    OP = OP - 1;                                                                    \
    reg->X = OP;                                                                    \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}


#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "DEX_def.h"
