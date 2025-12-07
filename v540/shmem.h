#ifndef __SHMEM_H__
#define __SHMEM_H__

#include "hw_c2-4p.h"

int shm_connect(paint_all_t cb1, paint_char_t cb2);
int kbshm_connect(size_t size) ;
int shm_disconnect(void);
void* shm_cmd_loop(void*);
struct video540_t* shm_get_mbx(void);
fifo_v500_kbd_t* shkb_get_fifo(void);
void shm_update_finalize(void);
struct video540_t* shm_create_mbx(int size,uint8_t* vmem_ptr) ;
fifo_v500_kbd_t* kbshm_create_fifo( size_t size );
#endif
