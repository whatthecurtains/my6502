
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define RTI(mode,cc) \
all_regs_t* RTI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,0)                                                            \
    reg->SR_all = POP;                                                              \
    uint16_t tgt = POP;                                                             \
    tgt |= ((uint16_t)POP)<<8;                                                      \
    reg->PC = tgt+1;                                                                \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "RTI_def.h"
