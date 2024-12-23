#include "cpu.h"
#include "memory.h"

typedef enum {
    OK,
    ERROR
} ExitCode;

ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_memory mem, rv_instruction instr);
