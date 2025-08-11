#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shmem.h"
#include "hw_c2-4p.h"

int shm=-1;
struct video540_t* ptr=NULL;

paint_all_t paint_all=NULL;
paint_char_t paint_char=NULL;

int shm_connect(paint_all_t cb1, paint_char_t cb2) {
    shm=shm_open("OSI540-share",O_RDWR,S_IRUSR | S_IWUSR);
    //shm=open("./OSI540-share",O_RDWR|O_DSYNC,0666);
    if (shm!=-1) {
        ptr = mmap(NULL,sizeof(struct video540_t),PROT_READ | PROT_WRITE,MAP_SHARED,shm,0);
    }
    else {
        printf("Error: %s\n",strerror(errno));
        return 1;
    }
    if (!ptr) {
        printf("Error: %s\n",strerror(errno));
        return 1;
    }
    paint_all = cb1;
    paint_char= cb2;
    return 0;
}

struct video540_t* shm_get_mbx(void) {
    return ptr;
}

int shm_disconnect() {
    munmap(ptr,sizeof(struct video540_t));
    shm_unlink("OSI540-share");
    //close(shm);
}

volatile int final=0;

void shm_update_finalize( void ) {
    final = 1;
}

void* shm_cmd_loop(void* nothing) {
    int done = 0;
    if (ptr) {
        while (!done) {
            switch (ptr->cmd) {
            case VMEM_ALL:
                paint_all();
                final = 0;
                while(!final);
                ptr->cmd = VMEM_IDLE;
                break;
            case VMEM_BYTE:
                paint_char();
                final = 0;
                while(!final);
                ptr->cmd = VMEM_IDLE;
                break;
            case VMEM_CLOSE:
                done = 1;
                break;
            default:
                usleep(1);
                break;
            }
        }
    }
    shm_disconnect();
    return NULL;
}
