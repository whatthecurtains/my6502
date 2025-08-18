#ifndef __SHMEM_H__
#define __SHMEM_H__

#include "hw_c2-4p.h"

int shm_connect(paint_all_t cb1, paint_char_t cb2);
int shm_disconnect(void);
void* shm_cmd_loop(void*);
struct video540_t* shm_get_mbx(void);
void shm_update_finalize(void);
struct video540_t* shm_create_mbx(int size,uint8_t* vmem_ptr) ;
#endif
