
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define SEI(mode,cc) \
all_regs_t* SEI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    reg->SR.I = 1;                                                                      \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}


#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "SEI_def.h"
