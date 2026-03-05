#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shmem.h"
#include "hw_c2-4p.h"
#include "fifo.h"
#include "shmfifo.h"


implement_fifo(v540_update)




int shm=-1;
struct video540_t* ptr=NULL;

int shkb=-1;
fifo_v500_kbd_t* kbptr=NULL;

size_t fifo_size=0;

paint_all_t paint_all=NULL;
paint_char_t paint_char=NULL;

implement_shmfifo(v500_kbd)

////////////////////////////////////////////////////////////////////////////////
// Called from the video process (child)

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

int kbshm_connect(size_t size) {
/*    fifo_size=sizeof(fifo_v500_kbd_t)+size*sizeof(v500_kbd);
    shkb = shm_open("OSI500_kbd", O_RDWR, S_IRUSR|S_IWUSR);
    if (shkb!=-1) {
        kbptr = (fifo_v500_kbd_t*) mmap(NULL,fifo_size,PROT_READ | PROT_WRITE,MAP_SHARED,shkb,0);
    }
    else {
        printf("Error: %s connecting keyboard fifo\n",strerror(errno));
        return 1;
    }
    if (!kbptr) {
        printf("Error: %s connecting keyboard fifo\n",strerror(errno));
        return 1;
    }
    printf("(video) fifo size = %ld\n", kbptr->size);
    return 0;
*/
    int retval= v500_kbd_shm_fifo_connect("OSI_v500_kbd",128, &kbptr);
    if (retval==-1) {
        printf("MMAP failed during shared memory fifo connect.\n");
    }
    else if ( retval<-1 ) {
        printf("kbdhm_connect: %s\n",strerror(retval));
    }
    return retval;
}

struct video540_t* shm_get_mbx(void) {
    return ptr;
}

fifo_v500_kbd_t* shkb_get_fifo(void) {
    return kbptr;
}

int shm_disconnect() {
    munmap(ptr,sizeof(struct video540_t));
    shm_unlink("OSI540-share");
/*    munmap(kbptr,fifo_size);
    shm_unlink("OSI500_kbd"); */
    v500_kbd_shm_fifo_disconnect("OSI_v500_kbd",128,kbptr);
    //close(shm);
}

volatile int final=0;

void shm_update_finalize( void ) {
    final = 1;
}

void* shm_cmd_loop(void* nothing) {
    int done = 0;
    uint64_t count;
    if (ptr) {
        while (!done) {
            while (!v540_update_empty(&ptr->vm_write)) {
                volatile v540_update* item = v540_update_tail(&ptr->vm_write);
                //printf("Received a command: ");
                //printf(" CMD %d : Addr %4.4X\n", item->cmd, item->addr);
                switch (item->cmd) {
                case VMEM_ALL:
                    paint_all();
                    final = 0;
                    count=0;
                    while(!final && count++ <(1<<10));
                    break;
                case VMEM_BYTE:
                    paint_char();
                    final = 0;
                    count=0;
                    while(!final && count++ <(1<<10));
                    break;
                case VMEM_CLOSE:
                    done = 1;
                    break;
                default:
                    //printf("  ** Not Recognized **\n");
                    v540_update_pop(&ptr->vm_write);
                }
            }
            //sleep(1);
        }
    }
    shm_disconnect();
    return NULL;
}

// Called from the client (read) side of the fifo
fifo_v500_kbd_t* kbshm_create_fifo( size_t size ) {
/*    int err;
    size_t fifo_size=sizeof(fifo_v500_kbd_t)+size*sizeof(v500_kbd);
    shkb = shm_open( "OSI500_kbd", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (shkb!=-1) {
        err=ftruncate(shkb, fifo_size);
        kbptr = (fifo_v500_kbd_t*) mmap(NULL,fifo_size,PROT_READ | PROT_WRITE,MAP_SHARED,shkb,0);
    }
    else {
        printf("Error %s\n",strerror(errno));
        return NULL;
    }
    if (!kbptr) {
        printf("Error %s\n",strerror(errno));
        return NULL;
    }
    v500_kbd_fifo_init(&kbptr,size);
    return kbptr;
*/

    int retval=v500_kbd_shm_fifo_create("OSI_v500_kbd",128,&kbptr);
    if (retval==-1)
        printf("MMAP failed during shared memory fifo connect.\n");
    else if (retval<0)
        printf("kbshm_create_fifo: %s\n",strerror(retval));
    return kbptr;
}

// Need to write the code here to create the file on the server (write) side of the fifo
// which will be called from the video app.

static int                  _shm=-1;
static struct video540_t*   _vmem=NULL;
static uint8_t*             gimage=NULL;

struct video540_t* shm_create_mbx(int size,uint8_t* vmem_ptr) {
    int err;
    char* e=NULL;
    pid_t vproc;
    size_t mbx_size=sizeof(struct video540_t)+(size)*sizeof(v540_update);
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


