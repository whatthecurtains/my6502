
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define DEC(mode,cc) \
all_regs_t* DEC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    OP = OP - 1;                                                                    \
    memwr(addr,OP);                                                                 \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "DEC_def.h"
