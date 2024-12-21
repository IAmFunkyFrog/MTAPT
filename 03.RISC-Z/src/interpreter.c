#include "interpreter.h"

// FIXME add -fwrapv compilation flag
static ExitCode execute_I_instruction(rv_cpu *cpu, rv_instruction instr) {
    unsigned int rd = instr.I_type.rd;
    unsigned int rs1 = instr.I_type.rs1;
    int imm = instr.I_type.imm;
    switch (instr.type_accessor.funct3)
    {
        default:
            break;
        case ADDI_funct3:
            cpu->x[rd] = cpu->x[rd] + imm;
            return OK;
    }
}

static ExitCode rv_cpu_cycle_helper(rv_cpu *cpu, rv_instruction instr) {
    switch (instr.type_accessor.opcode)
    {
        default:
            break;
        case ADDI_mask:
            return execute_I_instruction(cpu, instr);
    }
    return ERROR;
}


ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_instruction instr) {
    cpu->x[0] = 0; // Note: paranoic :)
    ExitCode code = rv_cpu_cycle_helper(cpu, instr);
    cpu->x[0] = 0;
    return code;
}
