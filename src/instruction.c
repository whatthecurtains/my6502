#include <stdio.h>
#include "instruction.h"


#define INSTR(inst,mode,cc,hex,bytes) \
    { inst##_##mode, #inst, #mode,hex,bytes },

opcode_table_t inst_tab[] = {
#include "all_instr.c"
};

opcode_table_t inst_decode[256];


void build_decode(void) {
    int i;
    for (i=0; i<sizeof(inst_decode)/sizeof(inst_decode[0]); i++) {
        inst_decode[i].hex = 0xFF;
        inst_decode[i].bytes = 1;
    }
    for (i=0; i<sizeof(inst_tab)/sizeof(inst_tab[0]); i++) {
        // printf("Adding instruction %s to the decode table\n",inst_tab[i].inst);
        inst_decode[inst_tab[i].hex] = inst_tab[i];
    }
}
