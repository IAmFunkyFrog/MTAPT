#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "interpreter.h"
#include "assert.h"

int create_memory_from_file(const char *filename, rv_memory *mem) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1; // TODO add normal error code

    fseek(file, 0, SEEK_END);
    size_t code_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    assert(code_size > 0);
    // TODO support data section
    // (1) create code section
    rv_memory_chunk *code = malloc(sizeof(rv_memory_chunk));
    *code = (rv_memory_chunk) {
        .memory = malloc(code_size),
        .start = 0x10000, // Note: hardcoded start point, change?
        .end = 0x10000 + code_size
    };

    // (2) create stack
    const int max_stack_size = 65356; // Note: enough for tests
    rv_memory_chunk *stack = malloc(sizeof(rv_memory_chunk));
    *stack = (rv_memory_chunk) {
        .memory = malloc(max_stack_size),
        .start = 0x7ffffff0 - max_stack_size, // Note: hardcoded venus setting
        .end = 0x7ffffff0
    };

    // (3) link sections
    code->next = stack;

    // (4) fill code from file
    fread(code->memory, 1, code_size, file);

    // (5) fill memory and return
    mem->first_chunk = code;
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    char *filename = NULL;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            break;
        default:
            fprintf(stderr, "TODO fill help");
            return -1;
        }
    }

    if (filename) {
        rv_memory mem;
        if (create_memory_from_file(filename, &mem) == -1) return -1;

        // Note: first chunk is always code start
        rv_cpu cpu = {.pc = mem.first_chunk->start};
        cpu.x[2] = mem.first_chunk->next->end; // FIXME set stack pointer hardcoded
        ExitCode code = rv_cpu_interpret_memory(&cpu, mem);
        assert(code == EBREAK_exit);
    }

    return 0;
}
