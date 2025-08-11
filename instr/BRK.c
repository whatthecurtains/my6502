
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define BRK(mode,cc) \
all_regs_t* BRK_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    uint8_t pc_low = (reg->PC+2) & 0xFF;                                                \
    uint8_t pc_high = (reg->PC+2)>>8;                                                   \
    PUSH(pc_low);                                                                       \
    PUSH(pc_high);                                                                      \
    PUSH(reg->SR_all);                                                                  \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "BRK_def.h"
