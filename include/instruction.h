#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "regs.h"
void build_decode(void);

extern opcode_table_t inst_decode[256];

#define INSTR(inst,mode,cc,hex,bytes)  \
    inst(mode,cc)

#include "protos.h"
#include "all_instr.c"

#undef INSTR

#endif
