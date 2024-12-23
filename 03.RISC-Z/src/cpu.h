#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef union {
    int encoding;
    /*struct {

    } R_type;*/
    struct {
        unsigned int opcode : 7;
        unsigned int rd : 5;
        unsigned int funct3 : 3;
        unsigned int rs1 : 5;
        int imm : 12;
    } I_type;
    struct {

    } S_type;
    struct {
        unsigned int opcode : 7;
        unsigned int imm11 : 1;
        unsigned int imm4_1 : 4;
        unsigned int funct3 : 3;
        unsigned int rs1 : 5;
        unsigned int rs2 : 5;
        unsigned int imm10_5 : 6;
        unsigned int imm12 : 1;
    } B_type;
    struct {
        unsigned int opcode : 7;
        unsigned int rd : 5;
        int imm : 20;
    } U_type;
    struct {
        unsigned int opcode : 7;
        unsigned int rd : 5;
        unsigned int imm19_12 : 8;
        unsigned int imm11 : 1;
        unsigned int imm10_1 : 10;
        unsigned int imm20 : 1;
    } J_type;
    struct {
        unsigned int opcode : 7;
        unsigned int unused2 : 5;
        unsigned int funct3 : 3;
        unsigned int unused1 : 10;
        unsigned int funct7 : 7;
    } type_accessor;
} rv_instruction;

#define REGISTER_COUNT 32
#define INSTRUCTION_WIDTH 4

typedef struct {
    int x[REGISTER_COUNT]; // Note: RV32I contain 32 gp registers
    int pc;
} rv_cpu;

void rv_dump_cpu(rv_cpu *cpu);

static inline rv_instruction rv_instruction_from_int(int encoding) {
    return (rv_instruction){.encoding = encoding};
}

#define MISSING_FUNCT3 0
#define MISSING_FUNCT7 0

#define X_MACRO_INSTRUCTION(F) \
    F(LUI, 0b0110111, MISSING_FUNCT3, MISSING_FUNCT7) \
    F(AUIPC, 0b0010111, MISSING_FUNCT3, MISSING_FUNCT7) \
    F(JAL, 0b1101111, MISSING_FUNCT3, MISSING_FUNCT7) \
    F(JALR, 0b1100111, 0, MISSING_FUNCT7) \
    F(BEQ, 0b1100011, 0b000, MISSING_FUNCT7) \
    F(BNE, 0b1100011, 0b001, MISSING_FUNCT7) \
    F(BLT, 0b1100011, 0b100, MISSING_FUNCT7) \
    F(BGE, 0b1100011, 0b101, MISSING_FUNCT7) \
    F(BLTU, 0b1100011, 0b110, MISSING_FUNCT7) \
    F(BGEU, 0b1100011, 0b111, MISSING_FUNCT7) \
    F(LB, 0b0000011, 0b000, MISSING_FUNCT7) \
    F(ADDI, 0b0010011, 0, MISSING_FUNCT7)

#define STR_CAT(x, y) x ## y

#define ENCODE_INSTR(name, opcode, funct3, funct7) \
    typedef enum { \
        STR_CAT(name, _opcode) = opcode, \
        STR_CAT(name, _funct3) = funct3, \
        STR_CAT(name, _funct7) = funct7, \
    } name;

X_MACRO_INSTRUCTION(ENCODE_INSTR)

#undef ENCODE_INSTR
#undef STR_CAT