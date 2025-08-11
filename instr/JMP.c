
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define JMP(mode,cc) \
all_regs_t* JMP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,0)                                                            \
    reg->PC = tgt;                                                                  \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "JMP_def.h"
