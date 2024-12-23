#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "interpreter.h"
#include "assert.h"

#define TEST_ONE_INSTR_START_PC_MEMORY(name, initializer, condition, start_pc, memory_initializer) \
    ExitCode name() { \
        rv_cpu cpu = {.pc = start_pc}; \
        rv_instruction instr = (rv_instruction) initializer; \
        memory_initializer \
        ExitCode code = rv_cpu_cycle(&cpu, mem, instr); \
        if (code != OK) {\
            rv_dump_cpu(&cpu); \
            return code; \
        } \
        if (condition) \
            return OK; \
        rv_dump_cpu(&cpu); \
        return ERROR; \
    }

#define TEST_ONE_INSTR_START_PC(name, initializer, condition, start_pc) \
    TEST_ONE_INSTR_START_PC_MEMORY(name, initializer, condition, start_pc, rv_memory mem;)

#define TEST_ONE_INSTR(name, instr_encoding, condition) \
    TEST_ONE_INSTR_START_PC(name, {.encoding = instr_encoding}, condition, 0)

#define TEST_ONE_INSTR_MEMORY(name, initializer, condition, memory_initializer) \
    TEST_ONE_INSTR_START_PC_MEMORY(name, initializer, condition, 0, memory_initializer)

// Note: address of word is 100
#define TEST_ONE_INSTR_MEMORY_ONE_WORD(name, initializer, condition, word) \
    TEST_ONE_INSTR_START_PC_MEMORY(name, initializer, condition, 0, \
        word; \
        rv_memory_chunk chunk; \
        chunk.memory = &data; \
        chunk.start = 100; \
        chunk.end = 100 + sizeof(data); \
        rv_memory mem = {.first_chunk = &chunk}; \
    )

#define COMMA ,

// addi x1, x0, 1
TEST_ONE_INSTR(TEST_ADDI_POSITIVE_IMM, 0x00100093, cpu.x[1] == 1)
// addi x1, x0, -1
TEST_ONE_INSTR(TEST_ADDI_NEGATIVE_IMM, 0xfff00093, cpu.x[1] == -1)
// lui x1, 1
TEST_ONE_INSTR(TEST_LUI, 0x000010b7, cpu.x[1] == 0x00001000)
// auipc x1, 1
TEST_ONE_INSTR_START_PC(TEST_AUIPC, {.encoding = 0x00001097}, cpu.x[1] == 0x00001004, 4)
// jal x1, -4
TEST_ONE_INSTR_START_PC(TEST_JAL_NEGATIVE_IMM, {.encoding = 0xffdff0ef}, cpu.pc == 0 && cpu.x[1] == 8, 4)
// jal x1, 8
TEST_ONE_INSTR_START_PC(TEST_JAL_POSITIVE_IMM, {.encoding = 0x008000ef}, cpu.pc == 12 && cpu.x[1] == 8, 4)
// jalr x1 x0 8
// TODO add test with no zero register as rs1
TEST_ONE_INSTR(TEST_JALR, 0x008000e7, cpu.pc == 8 && cpu.x[1] == 4)
// beq x0, x1, 8 where x1 == 0
// TODO add negative beq
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BEQ_JUMP, {.encoding = 0x00100463}; cpu.x[1] = 0, cpu.pc == 12, 4)
// beq x0, x1, 8 where x1 == 1
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BEQ_NO_JUMP, {.encoding = 0x00100463}; cpu.x[1] = 1, cpu.pc == 8, 4)
// bne x0, x1, 8 where x1 == 0
// TODO add negative bne
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BNE_JUMP, {.encoding = 0x00101463}; cpu.x[1] = 0, cpu.pc == 8, 4)
// bne x0, x1, 8 where x1 == 1
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BNE_NO_JUMP, {.encoding = 0x00101463}; cpu.x[1] = 1, cpu.pc == 12, 4)
// blt x0, x1, 8 where x1 == 0
// TODO add negative blt
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BLT_NO_JUMP, {.encoding = 0x00104463}; cpu.x[1] = 0, cpu.pc == 8, 4)
// blt x0, x1, 8 where x1 == 1
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BLT_JUMP, {.encoding = 0x00104463}; cpu.x[1] = 1, cpu.pc == 12, 4)
// bge x0, x1, 8 where x1 == 1
// TODO add negative bge
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BGE_NO_JUMP, {.encoding = 0x00105463}; cpu.x[1] = 1, cpu.pc == 8, 4)
// bge x0, x1, 8 where x1 == 0
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BGE_JUMP, {.encoding = 0x00105463}; cpu.x[1] = 0, cpu.pc == 12, 4)
// bltu x0, x1, 8 where x1 == -1
// TODO add negative bltu
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BLTU_JUMP, {.encoding = 0x00106463}; cpu.x[1] = -1, cpu.pc == 12, 4)
// bltu x0, x1, 8 where x1 == 0
TEST_ONE_INSTR_START_PC(TEST_POSITIVE_BLTU_NO_JUMP, {.encoding = 0x00106463}; cpu.x[1] = 0, cpu.pc == 8, 4)
// lb x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_LB, {.encoding = 0x00010083}; cpu.x[2] = 100, cpu.x[1] == -1,
    char data = -1
)
// lh x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_LH, {.encoding = 0x00011083}; cpu.x[2] = 100, cpu.x[1] == 0xFFFFbeef,
    short data = 0xbeef
)
// lw x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_LW, {.encoding = 0x00012083}; cpu.x[2] = 100, cpu.x[1] == 0xdeadbeef,
    int data = 0xdeadbeef
)
// lbu x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_LBU, {.encoding = 0x00014083}; cpu.x[2] = 100, cpu.x[1] == 255,
    char data = -1
)
// lhu x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_LHU, {.encoding = 0x00015083}; cpu.x[2] = 100, cpu.x[1] == 0xbeef,
    short data = 0xbeef
)
// sb x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_SB, {.encoding = 0x00110023}; cpu.x[2] = 100; cpu.x[1] = -1;, data == -1,
    char data = 0
)
// sh x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_SH, {.encoding = 0x00111023}; cpu.x[2] = 100; cpu.x[1] = 0xbeef;, data == 0xbeef,
    unsigned short data = 0
)
// sw x1, 0(x2)
TEST_ONE_INSTR_MEMORY_ONE_WORD(TEST_SW, {.encoding = 0x00112023}; cpu.x[2] = 100; cpu.x[1] = 0xdeadbeef;, data == 0xdeadbeef,
    int data = 0
)
// slti x1, x0, 1
TEST_ONE_INSTR(TEST_SLTI, 0x00102093, cpu.x[1] == 1)
// sltiu x1, x0, -1
TEST_ONE_INSTR(TEST_SLTIU, 0xfff03093, cpu.x[1] == 1)
// xori x1, x0, -1
TEST_ONE_INSTR(TEST_XORI, 0xfff04093, cpu.x[1] == -1)
// ori x1, x0, 1
TEST_ONE_INSTR(TEST_ORI, 0x00106093, cpu.x[1] == 1)
// andi x1, x2, 1
TEST_ONE_INSTR_START_PC(TEST_ANDI, {.encoding = 0x00117093}; cpu.x[2] = 3, cpu.x[1] == 1, 0)
// slli x1, x2, 1
TEST_ONE_INSTR_START_PC(TEST_SLLI, {.encoding = 0x00111093}; cpu.x[2] = 1, cpu.x[1] == 2, 0)
// srli x1, x2, 1
TEST_ONE_INSTR_START_PC(TEST_SRLI, {.encoding = 0x00115093}; cpu.x[2] = -1, cpu.x[1] == ((unsigned int) -1) >> 1, 0)
// srai x1, x2, 1
TEST_ONE_INSTR_START_PC(TEST_SRAI, {.encoding = 0x40115093}; cpu.x[2] = -1, cpu.x[1] == -1, 0)

#define TEST_FUNCTIONS(F) \
    F(TEST_ADDI_POSITIVE_IMM) \
    F(TEST_ADDI_NEGATIVE_IMM) \
    F(TEST_LUI) \
    F(TEST_AUIPC) \
    F(TEST_JAL_NEGATIVE_IMM) \
    F(TEST_JAL_POSITIVE_IMM) \
    F(TEST_JALR) \
    F(TEST_POSITIVE_BEQ_JUMP) \
    F(TEST_POSITIVE_BEQ_NO_JUMP) \
    F(TEST_POSITIVE_BNE_JUMP) \
    F(TEST_POSITIVE_BNE_NO_JUMP) \
    F(TEST_POSITIVE_BLT_JUMP) \
    F(TEST_POSITIVE_BLT_NO_JUMP) \
    F(TEST_POSITIVE_BGE_JUMP) \
    F(TEST_POSITIVE_BGE_NO_JUMP) \
    F(TEST_POSITIVE_BLTU_JUMP) \
    F(TEST_POSITIVE_BLTU_NO_JUMP) \
    F(TEST_LB) \
    F(TEST_LH) \
    F(TEST_LW) \
    F(TEST_LBU) \
    F(TEST_LHU) \
    F(TEST_SB) \
    F(TEST_SH) \
    F(TEST_SW) \
    F(TEST_SLTI) \
    F(TEST_SLTIU) \
    F(TEST_XORI) \
    F(TEST_ORI) \
    F(TEST_ANDI) \
    F(TEST_SLLI) \
    F(TEST_SRLI) \
    F(TEST_SRAI)

int main(int argc, char *argv[])
{
    int passed = 0;
    int failed = 0;
    char *result_str;

    printf("========== RUN TESTS:\n");
#define MAKE_TEST(name) \
    { \
        ExitCode code = name(); \
        if (code == ERROR) {\
            failed++; \
            result_str = "FAILED"; \
        }\
        else if (code == OK) {\
            passed++; \
            result_str = "PASSED"; \
        } \
        printf("Test %s: %s\n", #name, result_str); \
    }
    TEST_FUNCTIONS(MAKE_TEST)
#undef MAKE_TEST

    printf("Test passed %d/%d\n", passed, passed + failed);
    return failed;
}
