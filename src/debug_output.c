
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction.h"
#include "bus.h"
#include "debug_output.h"

typedef enum { NO_ADDR,SHORT_ADDR,LONG_ADDR,REL_ADDR} addr_t;

typedef struct {
    const char* mode;
    const char* fmt;
    addr_t      addr_mode_special;
} debug_output_t;

debug_output_t debug_mode_tbl[] = {
    {"imp","%s        ",NO_ADDR},
    {"acc","%s        ",NO_ADDR},
    {"imm","%s #$%2.2X   ",SHORT_ADDR},
    {"abs","%s $%2.2X%2.2X  ",LONG_ADDR},
    {"absjmp","%s $%2.2X%2.2X  ",LONG_ADDR},
    {"abs_x","%s $%2.2X%2.2X,X",LONG_ADDR},
    {"abs_y","%s $%2.2X%2.2X,Y",LONG_ADDR},
    {"ind","%s $(%2.2X%2.2X)",LONG_ADDR},
    {"zpg","%s $%2.2X    ",SHORT_ADDR},
    {"zpg_x","%s $%2.2X,X  ",SHORT_ADDR},
    {"zpg_y","%s $%2.2X,Y  ",SHORT_ADDR},
    {"x_ind","%s $(%2.2X,X)",SHORT_ADDR},
    {"ind_y","%s $(%2.2X),Y",SHORT_ADDR},
    {"rel","%s $%4.4X  ",REL_ADDR},
    {"absind","%s $(%4.4X)",LONG_ADDR}
};

FILE* debug_file=NULL;

void dump_regs(all_regs_t* R){
    DBG_OUT( "    PC: $%4.4X A: $%2.2X  X: $%2.2X  Y: $%2.2X SP:$%2.2X SR[N:%d V:%d B:%d D:%d I:%d Z:%d C:%d]\n",
            R->PC,R->A,R->X,R->Y,R->SP,R->SR.N,R->SR.V,R->SR.B,R->SR.D,R->SR.I,R->SR.Z,R->SR.C);
}

void dbg_out(opcode_table_t* op, all_regs_t* reg,int dis) {
    debug_output_t* ptbl=NULL;
    if (!dis && op->hex==0xFF) {
        DBG_OUT("*Error* FATAL Ilegal instruction");
        dump_regs(reg);
        fclose(debug_file);
        printf("*Error* FATAL Ilegal instruction");
        //exit(-1);
        while(1);
    }
    if (dis && op->hex==0xFF) {
        DBG_OUT("0x%4.4X %2.2X       \n",reg->PC, memrd(reg->PC));
        return;
    }
    for (int i=0;i<sizeof(debug_mode_tbl)/sizeof(debug_mode_tbl[0]);i++) {
        if (strcmp(op->mode,debug_mode_tbl[i].mode)==0) ptbl = &debug_mode_tbl[i];
    }
    if (!dis && ptbl==NULL) {
        DBG_OUT("*Error* FATAL Ilegal address mode\n");
        dump_regs(reg);
        fflush(debug_file);
        exit(-1);
    }
    switch(ptbl->addr_mode_special) {
    case NO_ADDR:
        DBG_OUT("0x%4.4X %2.2X       ",reg->PC, op->hex);
        DBG_OUT(ptbl->fmt,op->inst);
        break;
    case SHORT_ADDR:
        DBG_OUT("0x%4.4X %2.2X %2.2X    ",reg->PC, op->hex, memrd(reg->PC+1));
        DBG_OUT(ptbl->fmt,op->inst,memrd(reg->PC+1));
        break;
    case LONG_ADDR:
        DBG_OUT("0x%4.4X %2.2X %2.2X %2.2X ",reg->PC, op->hex,memrd(reg->PC+1),memrd(reg->PC+2));
        DBG_OUT(ptbl->fmt,op->inst,memrd(reg->PC+2),memrd(reg->PC+1));
        break;
    case REL_ADDR:
        DBG_OUT("0x%4.4X %2.2X %2.2X    ",reg->PC, op->hex,memrd(reg->PC+1));
        DBG_OUT(ptbl->fmt,op->inst, reg->PC+2+(int8_t)memrd(reg->PC+1));
        break;
    }
    if (!dis) dump_regs(reg);
    else DBG_OUT("\n");
    DBG_FLUSH;
}


void begin_dbg( void ) {
    DBG_OPEN;
}

void end_dbg( void ) {
    DBG_CLOSE;
}
