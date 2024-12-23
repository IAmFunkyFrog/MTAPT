#include "assert.h"
#include "stdlib.h"

typedef struct memory_chunk {
    void *memory;
    int start;
    int end;
    struct memory_chunk *next;
} rv_memory_chunk;

typedef struct {
    rv_memory_chunk *first_chunk;
} rv_memory;

void *rv_memory_get_ptr(rv_memory mem, int addr);

static inline unsigned char rv_memory_read_byte(rv_memory mem, int addr) {
    void *ptr = rv_memory_get_ptr(mem, addr);
    assert(ptr != NULL);

    return *((unsigned char *) ptr);
}

static inline unsigned short rv_memory_read_short(rv_memory mem, int addr) {
    void *ptr = rv_memory_get_ptr(mem, addr);
    assert(ptr != NULL);

    return *((unsigned short *) ptr);
}

static inline unsigned int rv_memory_read_int(rv_memory mem, int addr) {
    void *ptr = rv_memory_get_ptr(mem, addr);
    assert(ptr != NULL);

    return *((unsigned int *) ptr);
}