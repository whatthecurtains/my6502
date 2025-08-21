#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shmem.h"
#include "hw_c2-4p.h"

implement_fifo(v540_update)


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
            while (!v540_update_empty(&ptr->vm_write)) {
                v540_update* item = v540_update_tail(&ptr->vm_write);
                //printf("Received a command: ");
                //printf(" CMD %d : Addr %4.4X\n", item->cmd, item->addr);
                switch (item->cmd) {
                case VMEM_ALL:
                    paint_all();
                    final = 0;
                    while(!final);
                    break;
                case VMEM_BYTE:
                    paint_char();
                    final = 0;
                    while(!final);
                    break;
                case VMEM_CLOSE:
                    done = 1;
                    break;
                default:
                    //printf("  ** Not Recognized **\n");
                    v540_update_pop(&ptr->vm_write);
                }
            }
            usleep(1);
        }
    }
    shm_disconnect();
    return NULL;
}


static int                  _shm=-1;
static struct video540_t*   _vmem=NULL;
static uint8_t*             gimage=NULL;


struct video540_t* shm_create_mbx(int size,uint8_t* vmem_ptr) {
    int err;
    char* e=NULL;
    pid_t vproc;
    size_t mbx_size=sizeof(struct video540_t)+size*sizeof(v540_update);
    _shm=shm_open("OSI540-share",O_CREAT|O_RDWR,S_IRUSR | S_IWUSR);
    if (_shm!=-1) {
        err=ftruncate(_shm,mbx_size);
        _vmem = (struct video540_t*)mmap(NULL,mbx_size,PROT_READ | PROT_WRITE,MAP_SHARED,_shm,0);
    }
    else {
        printf("Error %s\n",strerror(errno));
        return NULL;
    }
    if (!_vmem) {
        printf("Error %s\n",strerror(errno));
        return NULL;
    }
    v540_update_fifo_init_at(&_vmem->vm_write,size);
    if (vmem_ptr!=NULL) {
        memcpy(_vmem->vm,vmem_ptr,sizeof(_vmem->vm));
    }
    if ( (e=getenv("OSI_DISPLAY")) && (strncmp(e,"NONE",4)!=0)) {
        vproc = fork();
        switch(vproc) {
        case 0:         // Child
            err=execv("./video", (char*[]){"video",NULL});
            if (err) {
                printf("Error: %s\n",strerror(errno));
                exit(-1);
            }
            break;
        case -1:        // failed
            printf("Failed to create video hw process");
            exit(-1);
            break;
        default:
            printf("Video HW process id: %d\n",vproc);
            break;
        }
    }
    else {
        printf("Skipping video\n");
    }
    return _vmem;
}

