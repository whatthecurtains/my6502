
#include <stdint.h>
#include "bus.h"
#include "address_mode.h"

#define ADC(mode,cc) \
all_regs_t* ADC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc) { \
    mode(reg,low,high,1)                                                            \
    uint16_t sum = 0;                                                               \
    if (reg->SR.D) {                                                                \
        OP = OP/16*10 + OP & 0xF;                                                   \
        sum = (reg->A/16*10 + reg->A&0xF) + OP + reg->SR.C;                         \
    }                                                                               \
    else {                                                                          \
        sum = reg->A + OP + (reg->SR.C ? 1 :0);                                     \
    }                                                                               \
    reg->A = (uint8_t)(sum & 0xFF);                                                 \
    reg->SR.N = (sum&0x80)!=0 ? 1 : 0;                                              \
    reg->SR.V = ((reg->A ^ sum)&(OP^sum))&0x80 == 0x80 ? 1 : 0;                     \
    reg->SR.Z = (sum==0) ? 1 : 0;                                                   \
    reg->SR.C = (sum&0x100) ? 1 : 0;                                                \
    *cyc += (cc);                                                                   \
    return reg;                                                                     \
}

#define INSTR(inst,mode,cyc,hex,byte)   \
    inst(mode,cyc)

#include "ADC_def.h"
