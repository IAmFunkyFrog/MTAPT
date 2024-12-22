#include "interpreter.h"
#include <assert.h>

static ExitCode execute_I_instruction(rv_cpu *cpu, rv_instruction instr) {
    unsigned int rd = instr.I_type.rd;
    unsigned int rs1 = instr.I_type.rs1;
    int imm = instr.I_type.imm;
    int pc_before = cpu->pc;
    cpu->pc += INSTRUCTION_WIDTH;
    int pc_after = cpu->pc;
    switch (instr.type_accessor.opcode)
    {
        default:
            break;
        case JALR_opcode:
            assert(instr.type_accessor.funct3 == JALR_funct3);
            cpu->x[rd] = pc_after;
            cpu->pc = rs1 + imm;
            return OK;
        case ADDI_opcode:
            assert(instr.type_accessor.funct3 == ADDI_funct3);
            cpu->x[rd] = cpu->x[rd] + imm;
            return OK;
    }
}

static ExitCode execute_U_instruction(rv_cpu *cpu, rv_instruction instr) {
    unsigned int rd = instr.U_type.rd;
    int imm = instr.U_type.imm << 12;
    int pc = cpu->pc;
    cpu->pc += INSTRUCTION_WIDTH;
    switch (instr.type_accessor.opcode)
    {
        default:
            return ERROR;
        case AUIPC_opcode:
            cpu->x[rd] = pc + imm;
            return OK;
        case LUI_opcode:
            cpu->x[rd] = imm;
            return OK;
    }
    return ERROR;
}

static ExitCode execute_J_instruction(rv_cpu *cpu, rv_instruction instr) {
    unsigned int rd = instr.J_type.rd;
    unsigned int read_imm = instr.J_type.imm19_12 << 12 |
              instr.J_type.imm11 << 11 |
              instr.J_type.imm10_1 << 1 |
              instr.J_type.imm20 << 20;
    int imm = ((int) read_imm << 12) >> 12; // Note: sign extend read_imm to integer
    int pc_before = cpu->pc;
    cpu->pc += INSTRUCTION_WIDTH;
    int pc_after = cpu->pc;
    switch (instr.type_accessor.opcode)
    {
        default:
            return ERROR;
        case JAL_opcode:
            cpu->x[rd] = pc_after;
            cpu->pc = pc_before + imm;
            return OK;
    }
    return ERROR;
}

static ExitCode rv_cpu_cycle_helper(rv_cpu *cpu, rv_instruction instr) {
    switch (instr.type_accessor.opcode)
    {
        default:
            break;
        case LUI_opcode:
        case AUIPC_opcode:
            return execute_U_instruction(cpu, instr);
        case JAL_opcode:
            return execute_J_instruction(cpu, instr);
        case JALR_opcode:
        case ADDI_opcode:
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
