#include "cpu.h"
#include "memory.h"

typedef enum {
    OK,
    ERROR,
    EBREAK_exit,
} ExitCode;

ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_memory mem, rv_instruction instr);
ExitCode rv_cpu_interpret_memory(rv_cpu *cpu, rv_memory mem);
