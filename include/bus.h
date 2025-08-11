
#ifndef __BUS_H__
#define __BUS_H__

#include <stdint.h>
#include "regs.h"

typedef int BOOL;
#define TRUE  1
#define FALSE 0

typedef enum { INVALID ,BIN, MONITOR, MAP  } image_t;

typedef struct {
    uint16_t    begin;
    uint16_t    end;
} map_entry_t;

typedef struct {
    uint8_t     count;
    map_entry_t map[];
} map_t;


void    memwr( uint16_t addr, uint8_t data );
uint8_t memrd( uint16_t addr );

uint16_t load_image( const char* name, image_t type );
void snapshot( const char* name, all_regs_t* state );
void close_image( int persist );
map_t* get_image_map(void);
BOOL in_map(all_regs_t* R);
all_regs_t* get_regs(void);
int getIRQstate();
int getNMIstate();
#endif
