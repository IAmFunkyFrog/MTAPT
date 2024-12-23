#include "stdlib.h"
#include "memory.h"

void *rv_memory_get_ptr(rv_memory mem, int addr) {
    for (rv_memory_chunk *chunk = mem.first_chunk; chunk != NULL; chunk = chunk->next) {
        if (addr >= chunk->start && addr < chunk->end) return chunk->memory + (addr - chunk->start);
    }

    return NULL;
}
