
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define CLD(mode,cc) \
all_regs_t* CLD_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    reg->SR.D = 0;                                                                      \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "CLD_def.h"
