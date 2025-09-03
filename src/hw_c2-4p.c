
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include "bus.h"
#include "hw_model.h"
#include <stdlib.h>
#include "hw_c2-4p.h"

implement_fifo(v540_update)

int myfunc(void) {
    printf("This is a function\n");
    return 3;
}

#define B540_BASE  (0xD000)
#define B540_SIZE  (0x0800)

#define ACIA_BASE (0xC000)
#define ACIA_SIZE (0x10)

#define KBD_BASE  (0xC200)
#define KBD_SIZE  (0x10)

#define PIO_BASE  (0xC800)
#define PIO_SIZE  (0x10)

#define PTC_BASE  (0xC810)
#define PTC_SIZE  (0x10)


#define ADDR_IN(addr,base,size)  ((addr&(0xFFFF^(size-1)))==base)

#define IS_540(addr)    ADDR_IN(addr,B540_BASE,B540_SIZE)
#define IS_ACIA(addr)   ADDR_IN(addr,ACIA_BASE,ACIA_SIZE)
#define IS_KBD(addr)    ADDR_IN(addr,KBD_BASE,KBD_SIZE)
#define IS_PIO(addr)    ADDR_IN(addr,PIO_BASE,PIO_SIZE)
#define IS_PTC(addr)    ADDR_IN(addr,PTC_BASE,PTC_SIZE)


void    B540_init ( uint8_t* image);
void    B540_write( uint16_t addr, uint8_t data);
uint8_t B540_read ( uint16_t addr );
void    B540_halt ( uint8_t* image);

void    ACIA_init ( uint8_t* image);
void    ACIA_write( uint16_t addr, uint8_t data);
uint8_t ACIA_read ( uint16_t addr );
void    ACIA_halt ( uint8_t* image);

void    KBD_init ( uint8_t* image);
void    KBD_write( uint16_t addr, uint8_t data);
uint8_t KBD_read ( uint16_t addr );
void    KBD_halt ( uint8_t* image);

void    PIO_init ( uint8_t* image);
void    PIO_write( uint16_t addr, uint8_t data);
uint8_t PIO_read ( uint16_t addr );
void    PIO_halt ( uint8_t* image);

void    PTC_init ( uint8_t* image);
void    PTC_write( uint16_t addr, uint8_t data);
uint8_t PTC_read ( uint16_t addr );
void    PTC_halt ( uint8_t* image);

int ishw(uint16_t addr) {
    if (
        IS_540(addr)  ||
        IS_ACIA(addr) ||
        IS_KBD(addr)  ||
        IS_PIO(addr)  ||
        IS_PTC(addr)
    ) return TRUE;
    else return FALSE;
}


void hw_write(uint16_t addr, uint8_t data) {
    //printf("Writing $%4.4X with $%2.2X\n",addr,data);
    if      (IS_540(addr))  B540_write( addr, data);
    else if (IS_ACIA(addr)) ACIA_write( addr, data);
    else if (IS_KBD(addr))  KBD_write( addr, data);
    else if (IS_PIO(addr))  PIO_write( addr, data);
    else if (IS_PTC(addr))  PTC_write( addr, data);
}


uint8_t hw_read(uint16_t addr) {
    if      (IS_540(addr))  return B540_read(addr);
    else if (IS_ACIA(addr)) return ACIA_read(addr);
    else if (IS_KBD(addr))  return KBD_read(addr);
    else if (IS_PIO(addr))  return PIO_read(addr);
    else if (IS_PTC(addr))  return PTC_read(addr);
}



int         		_shm=-1;
struct video540_t* 	_vmem=NULL;
uint8_t*            gimage=NULL;

void hw_init(uint8_t* image) {
    B540_init(image);
    ACIA_init(image);
    KBD_init(image);
    PIO_init(image);
    PTC_init(image);
    // start HW thread
}


void hw_halt(uint8_t* image) {
    B540_halt(image);
    ACIA_init(image);
    KBD_init(image);
    PIO_init(image);
    PTC_init(image);
}


void    B540_init (uint8_t* image) {
    int err;
    char* e;
    pid_t vproc;
    gimage = image;
    size_t size=64;
    size_t mbx_size=sizeof(struct video540_t)+(size)*sizeof(v540_update);
    printf("sizeof(video540_t): %ld\nsizeof(fifo_v540_update_t): %ld\nsizeof(v540_update): %ld\nsize: %ld\nmbx_size: %ld\n",
        sizeof(struct video540_t),sizeof(fifo_v540_update_t), sizeof(v540_update),size,mbx_size);
    printf("FIFO size is %ld elements, mbx size is %ld\n",size,mbx_size);
    fflush(stdout);


    _shm=shm_open("OSI540-share",O_CREAT|O_RDWR,S_IRUSR | S_IWUSR);
    //_shm=open("./OSI540-share",O_CREAT|O_RDWR|O_DSYNC|O_TRUNC,0666);
    if (_shm!=-1) {
        err=ftruncate(_shm,mbx_size);
        _vmem = (struct video540_t*)mmap(NULL,mbx_size,PROT_READ | PROT_WRITE,MAP_SHARED,_shm,0);
        //v540_update_fifo_init_at(&_vmem->vm_write,256);
    }
    else {
        printf("Error: %s\n",strerror(errno));
        return;
    }
    if (!_vmem) {
        printf("Error: %s\n",strerror(errno));
        return;
    }
    v540_update_fifo_init_at(&_vmem->vm_write,size);
    memcpy(_vmem->vm,&image[B540_BASE],B540_SIZE);
    // use fifo here
    //_vmem->addr = 0;
    //_vmem->cmd=VMEM_ALL;

    printf("Launching video process\n");

    if ( (e=getenv("OSI_DISPLAY")) && (strncmp(e,"NONE",4)!=0)) {
        vproc = fork();
        switch(vproc) {
        case 0:         // Child
            err=execv("./video", (char*[]){"video",NULL});
            if (err) {
                printf("Error: %s\n",strerror(errno));
                exit(-1);
            }
            sleep(2);
            break;
        case -1:        // failed
            printf("Failed to create video hw process");
            exit(-1);
            break;
        default:
            sleep(2);
            printf("Video HW process id: %d\n",vproc);
            break;
        }
    }
    else {
        printf("Skipping video\n");
    }
}


void    B540_write(uint16_t addr, uint8_t data) {
    char * e;
    v540_update item;
    if ( (e=getenv("OSI_DISPLAY")) && (strncmp(e,"NONE",4)!=0)) {
        uint64_t count = 0;
        while (v540_update_full(&_vmem->vm_write)) {
            count++;
            if ( (count & 0xffff)==0 ) printf("Fifo full %ld\n",count++);
            usleep(1);
        }
        _vmem->vm[(addr&0x7FF)] = data;
        item.addr = addr;
        item.cmd = VMEM_BYTE;
        v540_update_push(&_vmem->vm_write,&item);
    }
}


uint8_t B540_read ( uint16_t addr ) {
    return gimage[addr];//_vmem->vm[addr];
}

void    B540_halt (uint8_t* image) {
    char* e;
    v540_update item;
    if ( (e=getenv("OSI_DISPLAY")) && (strncmp(e,"NONE",4)!=0)) {
        while (v540_update_full(&_vmem->vm_write)) {
            usleep(1);
        }
        item.cmd = VMEM_CLOSE;
        v540_update_push(&_vmem->vm_write,&item);
        while (!v540_update_empty(&_vmem->vm_write)) {
            usleep(1);
        }
    }
    shm_unlink("OSI540-share");
    //close(_shm);
}

void    ACIA_init ( uint8_t* image) {
}


void    ACIA_write( uint16_t addr, uint8_t data) {
}


uint8_t ACIA_read ( uint16_t addr ) {
    return 0;
}

void    ACIA_halt ( uint8_t* image) {
}

void    KBD_init ( uint8_t* image) {
}


void    KBD_write( uint16_t addr, uint8_t data) {
}


uint8_t KBD_read ( uint16_t addr ) {
    return 0;
}

void    KBD_halt ( uint8_t* image) {
}


void    PIO_init ( uint8_t* image) {
}


void    PIO_write( uint16_t addr, uint8_t data) {
}


uint8_t PIO_read ( uint16_t addr ) {
    return 0;
}

void    PIO_halt ( uint8_t* image) {
}


void    PTC_init ( uint8_t* image) {
}


void    PTC_write( uint16_t addr, uint8_t data) {
}


uint8_t PTC_read ( uint16_t addr ) {
    return 0;
}


void    PTC_halt ( uint8_t* image) {
}

