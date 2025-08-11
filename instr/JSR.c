
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define JSR(mode,cc) \
all_regs_t* JSR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,0)                                                            \
    PUSH((reg->PC+2)>>8);                                                           \
    PUSH((reg->PC+2)&0xFF);                                                         \
    reg->PC = tgt;                                                                  \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "JSR_def.h"
