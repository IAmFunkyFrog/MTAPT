#include "interpreter.h"
#include <assert.h>

static ExitCode execute_load(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
    assert(instr.type_accessor.opcode == LB_opcode);
    unsigned int rd = instr.I_type.rd;
    unsigned int rs1 = instr.I_type.rs1;
    int imm = instr.I_type.imm;

    switch (instr.type_accessor.funct3) {
        default:
            break;
        case LB_funct3: {
            unsigned char b = rv_memory_read_byte(mem, cpu->x[rs1] + imm);
            cpu->x[rd] = (int) ((b << 24) >> 24); // sign extension
            return OK;
        }
        case LH_funct3: {
            unsigned short s = rv_memory_read_short(mem, cpu->x[rs1] + imm);
            cpu->x[rd] = (int) ((s << 16) >> 16); // sign extension
            return OK;
        }
        case LW_funct3: {
            unsigned int i = rv_memory_read_int(mem, cpu->x[rs1] + imm);
            cpu->x[rd] = i;
            return OK;
        }
        case LBU_funct3: {
            unsigned char b = rv_memory_read_byte(mem, cpu->x[rs1] + imm);
            cpu->x[rd] = b;
            return OK;
        }
        case LHU_funct3: {
            unsigned short s = rv_memory_read_short(mem, cpu->x[rs1] + imm);
            cpu->x[rd] = s;
            return OK;
        }
    }
    return ERROR;
}

static ExitCode execute_R_instruction(rv_cpu *cpu, rv_instruction instr) {
    unsigned int rd = instr.R_type.rd;
    unsigned int rs1 = instr.R_type.rs1;
    unsigned int rs2 = instr.R_type.rs2;
    int imm = instr.R_type.imm;
    if (instr.R_type.opcode == SLLI_opcode) {
        switch (instr.type_accessor.funct3)
        {
            default:
                break;
            case SLLI_funct3: {
                unsigned int shift = rs2;
                cpu->x[rd] = cpu->x[rs1] << shift;
                return OK;
            }
            // case SRAI_funct3:
            case SRLI_funct3: {
                unsigned int shift = rs2;
                if (instr.type_accessor.funct7 == SRLI_funct7) cpu->x[rd] = ((unsigned int) cpu->x[rs1]) >> shift;
                else if (instr.type_accessor.funct7 == SRAI_funct7) cpu->x[rd] = cpu->x[rs1] >> shift;
                else return ERROR;
                return OK;
            }
        }
        return ERROR;
    }

    assert(instr.type_accessor.opcode == ADD_opcode);
    switch (instr.type_accessor.funct3)
    {
        default:
            break;
        // case SUB_opcode:
        case ADD_funct3: {
            if (instr.type_accessor.funct7 == ADD_funct7) cpu->x[rd] = cpu->x[rs1] + cpu->x[rs2];
            else if (instr.type_accessor.funct7 == SUB_funct7) cpu->x[rd] = cpu->x[rs1] - cpu->x[rs2];
            else return ERROR;
            return OK;
        }
        case SLL_funct3:
            cpu->x[rd] = cpu->x[rs1] << (cpu->x[rs2] & 0x1F);
            return OK;
        case SLT_funct3:
            if (cpu->x[rs1] < cpu->x[rs2]) cpu->x[rd] = 1;
            else cpu->x[rd] = 0;
            return OK;
        case SLTU_funct3:
            if (((unsigned int) cpu->x[rs1]) < ((unsigned int) cpu->x[rs2])) cpu->x[rd] = 1;
            else cpu->x[rd] = 0;
            return OK;
        case XOR_funct3:
            cpu->x[rd] = cpu->x[rs1] ^ cpu->x[rs2];
            return OK;
        // case SRA_funct3
        case SRL_funct3: {
            if (instr.type_accessor.funct7 == SRL_funct7) cpu->x[rd] = ((unsigned int) cpu->x[rs1]) >> (cpu->x[rs2] & 0x1F);
            else if (instr.type_accessor.funct7 == SRA_funct7) cpu->x[rd] = cpu->x[rs1] >> (cpu->x[rs2] & 0x1F);
            else return ERROR;
            return OK;
        }
        case OR_funct3:
            cpu->x[rd] = cpu->x[rs1] | cpu->x[rs2];
            return OK;
        case AND_funct3:
            cpu->x[rd] = cpu->x[rs1] & cpu->x[rs2];
            return OK;
    }
    return ERROR;
}

static ExitCode execute_arithmetic_with_imm_or_R_instruction(rv_cpu *cpu, rv_instruction instr) {
    assert(instr.type_accessor.opcode == ADDI_opcode);
    unsigned int rd = instr.I_type.rd;
    unsigned int rs1 = instr.I_type.rs1;
    int imm = instr.I_type.imm;
    switch (instr.type_accessor.funct3)
    {
        default:
            break;
        case ADDI_funct3:
            cpu->x[rd] = cpu->x[rs1] + imm;
            return OK;
        case SLTI_funct3:
            if (cpu->x[rs1] < imm) cpu->x[rd] = 1;
            else cpu->x[rd] = 0;
            return OK;
        case SLTIU_funct3:
            if (((unsigned int) cpu->x[rs1]) < ((unsigned int) imm)) cpu->x[rd] = 1;
            else cpu->x[rd] = 0;
            return OK;
        case XORI_funct3:
            cpu->x[rd] = cpu->x[rs1] ^ imm;
            return OK;
        case ORI_funct3:
            cpu->x[rd] = cpu->x[rs1] | imm;
            return OK;
        case ANDI_funct3:
            cpu->x[rd] = cpu->x[rs1] & imm;
            return OK;
        case SLLI_funct3:
        case SRLI_funct3:
        // case SRAI_funct3:
            return execute_R_instruction(cpu, instr);
    }
    return ERROR;
}

static ExitCode execute_I_instruction(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
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
            cpu->pc = cpu->x[rs1] + imm;
            return OK;
        case ADDI_opcode:
        // case SLTI_opcode:
        // case SLTIU_opcode:
        // case XORI_opcode:
        // case ORI_opcode:
        // case ANDI_opcode:
        // case SLLI_opcode:
        // case SRLI_opcode:
        // case SRAI_opcode:
            return execute_arithmetic_with_imm_or_R_instruction(cpu, instr);
        case LB_opcode:
        // case LH_opcode:
        // case LW_opcode:
        // case LBU_opcode:
        // case LHU_opcode:
            return execute_load(cpu, mem, instr);
    }
    return ERROR;
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

static ExitCode execute_B_instruction(rv_cpu *cpu, rv_instruction instr) {
    assert(instr.type_accessor.opcode == BEQ_opcode && "all B instructions should have such opcode");
    unsigned int rs1 = instr.B_type.rs1;
    unsigned int rs2 = instr.B_type.rs2;
    unsigned int read_imm = instr.B_type.imm4_1 << 1 |
              instr.B_type.imm11 << 11 |
              instr.B_type.imm10_5 << 5 |
              instr.B_type.imm12 << 12;
    int imm = ((int) read_imm << 20) >> 20; // Note: sign extend read_imm to integer
    int pc_before = cpu->pc;
    cpu->pc += INSTRUCTION_WIDTH;
    int pc_after = cpu->pc;
    switch (instr.type_accessor.funct3)
    {
        default:
            return ERROR;
        case BEQ_funct3:
            if (cpu->x[rs1] == cpu->x[rs2]) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
        case BNE_funct3:
            if (cpu->x[rs1] != cpu->x[rs2]) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
        case BLT_funct3:
            if (cpu->x[rs1] < cpu->x[rs2]) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
        case BGE_funct3:
            if (cpu->x[rs1] >= cpu->x[rs2]) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
        case BLTU_funct3:
            if (((unsigned int) cpu->x[rs1]) < ((unsigned int) cpu->x[rs2])) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
        case BGEU_funct3:
            if (((unsigned int) cpu->x[rs1]) >= ((unsigned int) cpu->x[rs2])) cpu->pc = pc_before + imm;
            else cpu->pc = pc_after;
            return OK;
    }
    return ERROR;
}

static ExitCode execute_S_instruction(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
    assert(instr.type_accessor.opcode == SB_opcode);
    unsigned int rs1 = instr.S_type.rs1;
    unsigned int rs2 = instr.S_type.rs2;
    unsigned int read_imm = instr.S_type.imm4_0 | instr.S_type.imm11_5 << 5;
    int imm = ((int) read_imm << 20) >> 20; // Note: sign extend read_imm to integer
    int pc_before = cpu->pc;
    cpu->pc += INSTRUCTION_WIDTH;
    int pc_after = cpu->pc;
    switch (instr.type_accessor.funct3)
    {
        default:
            return ERROR;
        case SB_funct3: {
            rv_memory_store_byte(mem, cpu->x[rs1] + imm, cpu->x[rs2]);
            return OK;
        }
        case SH_funct3: {
            rv_memory_store_short(mem, cpu->x[rs1] + imm, cpu->x[rs2]);
            return OK;
        }
        case SW_funct3: {
            rv_memory_store_int(mem, cpu->x[rs1] + imm, cpu->x[rs2]);
            return OK;
        }
    }
    return ERROR;
}

static ExitCode execute_ecall(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
    int imm = instr.I_type.imm;
    assert(imm == 0 && "EBRAK not supported yet");
    // Note: get ecalls from Venus https://github.com/61c-teach/venus/wiki/Environmental-Calls
    if (cpu->x[10] == 1) {
        // print_int
        printf("%d\n", cpu->x[11]);
        return OK;
    }
    return ERROR;
}

static ExitCode rv_cpu_cycle_helper(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
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
        // case SLTI_opcode:
        // case SLTIU_opcode:
        // case XORI_opcode:
        // case ORI_opcode:
        // case ANDI_opcode:
        // case SLLI_opcode:
        // case SRLI_opcode:
        // case SRAI_opcode:
        case LB_opcode:
        // case LH_opcode:
        // case LW_opcode:
        // case LBU_opcode:
        // case LHU_opcode:
            return execute_I_instruction(cpu, mem, instr);
        case BEQ_opcode:
        // case BNE_opcode:
            return execute_B_instruction(cpu, instr);
        case SB_opcode:
        // case SH_opcode:
        // case SW_opcode:
            return execute_S_instruction(cpu, mem, instr);
        case ADD_opcode:
        // case SUB_opcode:
        // case SLL_opcode:
        // case SLT_opcode:
        // case SLTU_opcode:
        // case XOR_opcode:
        // case SRL_opcode:
        // case SRA_opcode:
        // case OR_opcode:
        // case AND_opcode:
            return execute_R_instruction(cpu, instr);
        case ECALL_opcode:
            return execute_ecall(cpu, mem, instr);
    }
    return ERROR;
}


ExitCode rv_cpu_cycle(rv_cpu *cpu, rv_memory mem, rv_instruction instr) {
    cpu->x[0] = 0; // Note: paranoic :)
    ExitCode code = rv_cpu_cycle_helper(cpu, mem, instr);
    cpu->x[0] = 0;
    return code;
}
