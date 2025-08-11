#ifndef __HW_C2_4P_H__
#define __HW_C2_4P_H__

#include <stdint.h>


struct video540_t {
    uint8_t     vm[2048];
    uint16_t    addr;
    enum { VMEM_IDLE, VMEM_ALL, VMEM_BYTE, VMEM_CLOSE } cmd;
};


#endif
