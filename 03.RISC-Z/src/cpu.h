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
    /*struct {

    } S_type;
    struct {

    } B_type;
    struct {

    } U_type;
    struct {

    } J_type;*/
    struct {
        unsigned int opcode : 7;
        unsigned int unused2 : 5;
        unsigned int funct3 : 3;
        unsigned int unused1 : 10;
        unsigned int funct7 : 7;
    } type_accessor;
} rv_instruction;

#define REGISTER_COUNT 32

typedef struct {
    int x[REGISTER_COUNT]; // Note: RV32I contain 32 gp registers
} rv_cpu;

void rv_dump_cpu(rv_cpu *cpu);

inline rv_instruction rv_instruction_from_int(int encoding) {
    return (rv_instruction){.encoding = encoding};
}

#define MISSING_FUNCT3 0
#define MISSING_FUNCT7 0

#define X_MACRO_INSTRUCTION(F) \
    F(ADDI, 0b0010011, MISSING_FUNCT3, MISSING_FUNCT7)

#define STR_CAT(x, y) x ## y

#define ENCODE_INSTR(name, mask, funct3, funct7) \
    typedef enum { \
        STR_CAT(name, _mask) = mask, \
        STR_CAT(name, _funct3) = funct3, \
        STR_CAT(name, _funct7) = funct7, \
    } name;

X_MACRO_INSTRUCTION(ENCODE_INSTR)

#undef ENCODE_INSTR
#undef STR_CAT