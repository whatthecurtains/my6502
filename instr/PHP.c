
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define PHP(mode,cc) \
all_regs_t* PHP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) {     \
    mode(reg,low,high,0)                                                                \
    PUSH(reg->SR_all);                                                                  \
    *cyc += (cc);                                                                       \
    return reg;                                                                         \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
inst(mode,cyc)


#include "PHP_def.h"
