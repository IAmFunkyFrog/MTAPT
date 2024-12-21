#include "cpu.h"

typedef enum {
    OK,
    ERROR
} ExitCode;

ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_instruction instr);
