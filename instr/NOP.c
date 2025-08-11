
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"


#define NOP(mode,cc) \
all_regs_t* NOP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "NOP_def.h"
