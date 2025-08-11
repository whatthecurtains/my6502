
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define STX(mode,cc) \
all_regs_t* STX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,0)                                                            \
    memwr(addr,reg->X);                                                             \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "STX_def.h"
