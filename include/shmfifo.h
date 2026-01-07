#ifndef __SHMFIFO_H__
#define __SHMFIFO_H__


#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fifo.h"


#define implement_shmfifo(type)                                                                     \
                                                                                                    \
implement_fifo(type)                                                                                \
                                                                                                    \
static inline int type##_shm_fifo_create(const char* path, size_t num, fifo_##type##_t** q) {       \
    int fh=shm_open(path,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);                                      \
    if (fh<0) return fh;                                                                            \
    ftruncate(fh,num);                                                                              \
    size_t ms = sizeof(fifo_##type##_t)+num*sizeof(type);                                           \
    (*q)=(fifo_##type##_t*)mmap(NULL,ms,PROT_READ | PROT_WRITE, MAP_SHARED, fh, 0);                 \
    if (MAP_FAILED==(*q)) {                                                                         \
        close(fh);                                                                                  \
        return -1;                                                                                  \
    }                                                                                               \
    type##_fifo_init_at(*q,num);                                                                    \
    return fh;                                                                                      \
}                                                                                                   \
                                                                                                    \
static inline int type##_shm_fifo_connect(const char* path, size_t num, fifo_##type##_t** q) {      \
    static char expath[256];                                                                        \
    strncpy(expath,"/dev/shm/",sizeof(expath));                                                     \
    strncat(expath,path,sizeof(expath)-11);                                                         \
    while( access(expath,F_OK) !=0 ) {                                                              \
        usleep(1000);                                                                               \
    }                                                                                               \
                                                                                                    \
    int fh=shm_open(path,O_RDWR, S_IRUSR | S_IWUSR);                                                \
    if (fh<0) return fh;                                                                            \
    size_t ms = sizeof(fifo_##type##_t)+num*sizeof(type);                                           \
    (*q)=(fifo_##type##_t*)mmap(NULL,ms,PROT_READ | PROT_WRITE, MAP_SHARED, fh, 0);                 \
    if (MAP_FAILED==(*q)) {                                                                         \
        close(fh);                                                                                  \
        return -1;                                                                                  \
    }                                                                                               \
    return fh;                                                                                      \
}                                                                                                   \
                                                                                                    \
static inline int type##_shm_fifo_disconnect(const char* fh, size_t size, fifo_##type##_t* q) {     \
    shm_unlink(fh);                                                                                 \
    munmap((void*)q, sizeof(fifo_##type##_t)+q->size*sizeof(type));                                 \
    return 0;                                                                                       \
}                                                                                                   \

#endif // __SHMFIFO_H__
