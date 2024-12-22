#include "cpu.h"

void rv_dump_cpu(rv_cpu *cpu) {
    fprintf(stderr, "==== CPU DUMP:\n");
    fprintf(stderr, "pc: %d:\n", cpu->pc);
    for (int i = 0; i < REGISTER_COUNT; i++) {
        fprintf(stderr, "x%d: %d:\n", i, cpu->x[i]);
    }
    fprintf(stderr, "==== CPU DUMP END\n");
}