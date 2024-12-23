#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "interpreter.h"
#include "assert.h"

int main(int argc, char *argv[])
{
    rv_instruction instr = (rv_instruction) {.encoding = 0xfff00093};
    rv_cpu cpu = {};
    rv_memory mem;
    printf("%d\n", instr.type_accessor.opcode);
    rv_dump_cpu(&cpu);
    ExitCode code = rv_cpu_cycle(&cpu, mem, instr);
    assert(code == OK);
    rv_dump_cpu(&cpu);
    return 0;
}
