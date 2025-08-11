#ifndef __DEBUG_OUTPUT_H__
#define __DEBUG_OUTPUT_H__

#include <stdio.h>

#ifdef DEBUG_LOG
#define DBG_OUT(...) fprintf(debug_file,__VA_ARGS__)
#define DBG_OPEN debug_file=fopen("CPU_debug.txt","w");
#define DBG_FLUSH fflush(debug_file)
#define DBG_CLOSE fclose(debug_file)
#else
#define DBG_OUT(...) ;
#define DBG_OPEN
#define DBG_FLUSH
#define DBG_CLOSE
#endif
void dbg_out(opcode_table_t* op, all_regs_t* reg, int dis) ;
void begin_dbg( void );
void end_dbg( void ) ;
void dump_regs(all_regs_t*);
extern FILE* debug_file;
#endif
