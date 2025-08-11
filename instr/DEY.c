
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define DEY(mode,cc) \
all_regs_t* DEY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    OP=reg->Y;                                                                      \
    OP = OP - 1;                                                                    \
    reg->Y = OP;                                                                    \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "DEY_def.h"
