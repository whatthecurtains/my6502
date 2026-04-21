/*
 * bus_verify.c - Memory interface for opcode verification tests
 * 
 * Provides mock memrd/memrw functions for testing instructions
 * without relying on actual memory/hardware
 */

#include <stdint.h>
#include <string.h>

#define TEST_MEMORY_SIZE 0x10000
#define MAX_WRITE_HISTORY 16

static uint8_t test_memory[TEST_MEMORY_SIZE];
static struct {
    uint16_t addr;
    uint8_t data;
    int accessed;
} last_read, last_write;

/* Write history for tracking multiple writes (e.g., stack pushes in BRK) */
static struct {
    uint16_t addr;
    uint8_t data;
} write_history[MAX_WRITE_HISTORY];
static int write_history_count = 0;

/* Initialize test memory for verification test */
void bus_verify_init(void) {
    memset(test_memory, 0, sizeof(test_memory));
    memset(&last_read, 0, sizeof(last_read));
    memset(&last_write, 0, sizeof(last_write));
}

/* Reset memory access tracking */
void bus_verify_reset_tracking(void) {
    last_read.accessed = 0;
    last_write.accessed = 0;
    write_history_count = 0;
}

/* Mock memory read for testing */
uint8_t memrd(uint16_t addr) {
    uint8_t data = test_memory[addr];
    last_read.addr = addr;
    last_read.data = data;
    last_read.accessed = 1;
    return data;
}

/* Mock memory write for testing */
void memwr(uint16_t addr, uint8_t data) {
    test_memory[addr] = data;
    last_write.addr = addr;
    last_write.data = data;
    last_write.accessed = 1;
    /* Track write in history for multi-write instructions like BRK */
    if (write_history_count < MAX_WRITE_HISTORY) {
        write_history[write_history_count].addr = addr;
        write_history[write_history_count].data = data;
        write_history_count++;
    }
}

/* Get last memory read information */
void bus_verify_get_last_read(uint16_t *addr, uint8_t *data, int *accessed) {
    if (addr) *addr = last_read.addr;
    if (data) *data = last_read.data;
    if (accessed) *accessed = last_read.accessed;
}

/* Get last memory write information */
void bus_verify_get_last_write(uint16_t *addr, uint8_t *data, int *accessed) {
    if (addr) *addr = last_write.addr;
    if (data) *data = last_write.data;
    if (accessed) *accessed = last_write.accessed;
}

/* Set test memory value */
void bus_verify_set_memory(uint16_t addr, uint8_t data) {
    test_memory[addr] = data;
}

/* Get test memory value */
uint8_t bus_verify_get_memory(uint16_t addr) {
    return test_memory[addr];
}

/* Get write history count */
int bus_verify_get_write_history_count(void) {
    return write_history_count;
}

/* Get write history entry */
void bus_verify_get_write_history(int index, uint16_t *addr, uint8_t *data) {
    if (index >= 0 && index < write_history_count) {
        if (addr) *addr = write_history[index].addr;
        if (data) *data = write_history[index].data;
    }
}
