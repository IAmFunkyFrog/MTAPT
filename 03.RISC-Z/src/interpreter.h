#include "cpu.h"
#include "memory.h"

typedef enum {
    OK,
    ERROR,
    EBREAK_exit,
} ExitCode;

// Note: in RV spec defined that words in memory are little endian,
// but for our simulator it is simpler to store instruction as big endians
// FIXME!
typedef enum {
    LITTLE,
    BIG
} Endianess;

ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_memory mem, rv_instruction instr);
ExitCode rv_cpu_interpret_memory(rv_cpu *cpu, rv_memory mem, Endianess instruction_endianess);
