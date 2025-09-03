#ifndef __HW_C2_4P_H__
#define __HW_C2_4P_H__

#include "fifo.h"

typedef enum { VMEM_IDLE, VMEM_ALL, VMEM_BYTE, VMEM_CLOSE } v540_cmd_t;

typedef struct  {
    uint16_t    addr;
    v540_cmd_t  cmd;
} v540_update;

declare_fifo(v540_update)

struct video540_t {
    uint8_t             vm[2048];
    fifo_v540_update_t  vm_write;
};

typedef void (*paint_all_t)(void);
typedef void (*paint_char_t)(void);


#endif
