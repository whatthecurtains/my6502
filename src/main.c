
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "bus.h"
#include "instruction.h"
#include "debug_output.h"

uint64_t cycle = 0;
int boot( const char* image, image_t type) {
    begin_dbg();
    build_decode();
    load_image(image,type);
    all_regs_t* R = get_regs();
    memset(R,sizeof(*R),0);
    R->PC=memrd(0xFFFC) | memrd(0xFFFD)<<8;
    while ( !R->brk ) {
        if ( getNMIstate() ) {
            memwr( 0x100 + R->SP--, R->PC & 0xFF);
            memwr( 0x100 + R->SP--, R->PC >> 8);
            memwr( 0x100 + R->SP--, R->SP);
            R->PC = memrd( 0xFFFA ) | (memrd( 0xFFFB)<<8);
        }
        if ( getIRQstate() * R->SR.I ) {
            memwr( 0x100 + R->SP--, R->PC & 0xFF);
            memwr( 0x100 + R->SP--, R->PC >> 8);
            memwr( 0x100 + R->SP--, R->SP);
            R->PC = memrd( 0xFFFE ) | (memrd( 0xFFFF)<<8);
        }
        uint8_t op=memrd(R->PC );
        uint8_t low=memrd(R->PC +1);
        uint8_t high=memrd(R->PC +2);
        uint16_t pc=R->PC;
        dbg_out(&inst_decode[op],R,0);
        if (inst_decode[op].hex==0xFF) {
            DBG_OUT("*Error* FATAL Ilegal instruction");
            dump_regs(R);
            DBG_CLOSE;
            printf("*Error* FATAL Ilegal instruction");
            break;
        }
        inst_decode[op].opcf( R, low, high, &cycle );
        if (pc==R->PC) R->PC += inst_decode[op].bytes;
    }
    end_dbg();
    return 0;
}

void disassemble(const char* image, image_t type, uint16_t org) {
    map_t* mptr;
    begin_dbg();
    build_decode();
    uint16_t bytes = load_image(image,type);
    all_regs_t* R = get_regs();
    mptr  = get_image_map();
    uint8_t op = memrd(org);
    int done;
    if (mptr==NULL) {
        for (R->PC=org, done =0; !done; done=((uint32_t)R->PC+inst_decode[op].bytes)>>16, R->PC+=inst_decode[op].bytes) {
            op=memrd(R->PC);
            dbg_out(&inst_decode[op],R,1);
        }
    }
    else {
        R->PC = mptr->map[0].begin;
        while(in_map(R)) {
            uint16_t last_pc = R->PC;
            op=memrd(R->PC);
            dbg_out(&inst_decode[op],R,1);
            R->PC += inst_decode[op].bytes;
            if (R->PC<last_pc) break;
        }
    }
    end_dbg();
}

#define UPPER(x) ((x>='a'&&x<='z') ? x-'a'+'A' : x)
#define H2I(x) ((x>='A') ? x-'A'+10 : x-'0')

uint16_t hex2int(const char* hex) {
    const char* digit;
    int count=0;
    uint16_t retval=0;

    if (hex[1]=='x') digit = &hex[2];
    else digit = hex;
    while((count<4) && (digit[count]!=0)) {
        char x=digit[count++];
        int X;
        retval <<= 4;
        x=UPPER(x);
        X=H2I(x);
        retval |=  X;
    }
    return retval;
}

typedef enum {DISASSEMBLE, SIMULATE} exmode_t;

int main(int argc, char* const * argv) {
    int opt;
    const char* image_name;
    exmode_t xmode;
    image_t it=INVALID;
    uint16_t org;

    while((opt=getopt(argc,argv,"m:f:o:t:"))!=-1) {
        switch(opt){
            case 'm': if (strcmp(optarg,"disassemble")==0)
                          xmode = DISASSEMBLE;
                      else if (strcmp(optarg,"simulate")==0) {
                          xmode = SIMULATE;
                      }
                      else {
                          printf("---FAIL--- Illegal mode: %s",optarg);
                          exit(-1);
                      }
            break;
            case 'f': image_name = optarg;
            break;
            case 'o': org = hex2int(optarg);
            break;
            case 't': if (strcmp(optarg,"map")==0) {
                          it = MAP;
                      }
                      else if (strcmp(optarg,"binary")==0) {
                          it = BIN;
                      }
                      else if (strcmp(optarg,"monitor")==0) {
                          it = MONITOR;
                      }
                      else {
                          printf("---FAIL--- Illegal image type: %s",optarg);
                          exit(-1);
                      }
            break;
        }
    }
    if (xmode==DISASSEMBLE) {
        disassemble(image_name,it,org);
    }
    else if (xmode==SIMULATE) {
        boot(image_name,it);
    }

    return 0;
}
