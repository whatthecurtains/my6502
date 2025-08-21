#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "hw_c2-4p.h"
#include "shmem.h"

//#define CMD(x,a) {                          \
//    ptr->addr= a;                           \
//    ptr->cmd = x;                           \
//    count=0;                                \
//    while((ptr->cmd==x) && count<35) {      \
//        count++;                            \
//        usleep(30000);                      \
//    }                                       \
//    if (count==35)                          \
//        printf("*Error* cmd failed\n");     \
//}

v540_update item;

#define CMD(x,a) {                            \
    ptr->vm[a] = a;                           \
    item.addr = a;                            \
    item.cmd = x;                             \
    while (v540_update_full(&ptr->vm_write)) {\
        usleep(1);                            \
    }                                         \
    printf("CMD: %d, ADDR: %d\n", item.cmd, item.addr);    \
    v540_update_push(&ptr->vm_write,&item);   \
}

implement_fifo(v540_update)

int main( void ) {
    char line[1024];
    char cmd[256];
    char opt[256];
    uint32_t addr1;
    uint32_t addr2;
    uint32_t byte;
    int count;
    int err;
    int done=0;

    //int shm=shm_open("OSI540-share",O_CREAT|O_RDWR,S_IRUSR | S_IWUSR);
    //struct video540_t* ptr=NULL;
    //
    //if (shm!=-1) {
    //    err=ftruncate(shm,sizeof(struct video540_t));
    //    if (err) {
    //        printf("Error: %s\n",strerror(errno));
    //        return 1;
    //    }
    //    printf("Shared memory created\n");
    //    ptr = (struct video540_t*)mmap(NULL,sizeof(struct video540_t),PROT_READ | PROT_WRITE,MAP_SHARED,shm,0);
    //}
    //else {
    //    printf("Error: %s\n",strerror(errno));
    //    return 1;
    //}
    //
    //if (!ptr) {
    //    printf("Error: %s\n",strerror(errno));
    //    exit(-1);
    //}

    struct video540_t* ptr = shm_create_mbx(256,NULL);

    while (!done) {
        printf("> ");
        char* pline=line;
        uint64_t linelen=sizeof(line);
        count=getline(&pline,&linelen,stdin);
        if (4==sscanf(line,"%s %x %x %x", cmd, &addr1, &addr2, &byte)) {
            if (!strncmp("fill", cmd, 4)) {
                for (int i=addr1; i<(addr1+addr2); i++) {
                    ptr->vm[i] = byte;
                }
                CMD(VMEM_ALL,0);
                continue;
            }
        }

        if (3==sscanf(line,"%s %x %x",cmd,&addr1,&byte)) {
            if (!strncmp("set",cmd,3)) {
                ptr->vm[addr1] = byte;
            }
            CMD(VMEM_BYTE,addr1);
            continue;
        }

        if (2==sscanf(line,"%s %x",cmd,&byte)) {
            if (!strncmp("get",cmd,3)) {
                printf("0x%3.3X: 0x%2.2X\n", byte,ptr->vm[byte]);
                continue;
            }
        }

        if (2>sscanf(line,"%s",cmd)) {
            if (!strncmp("help",cmd,4)) {
                printf("Commands are:\n\tset hex_addr hex_byte\n\tget hex_addr\n\tfill hex_start hex_len hex_byte\n\tclear\n\tquit\n");
                continue;
            }
            if (!strncmp("clear",cmd,5)) {
                for (int i=0; i<0x800; i++) {
                    ptr->vm[i] = 0x20;
                }
                CMD(VMEM_ALL,0);
                continue;
            }
            if (!strncmp("quit",cmd,4)) {
                done = 1;
                continue;
            }
        }
        printf("Unrecognized command: %s\n",line);
    }
    shm_disconnect();

}
