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
TEST_ONE_INSTR_MEMORY(TEST_LB, {.encoding = 0x00010083}; cpu.x[2] = 100, cpu.x[1] == -1,
    int data = -1;
    rv_memory_chunk chunk;
    chunk.memory = &data;
    chunk.start = 100;
    chunk.end = 100 + sizeof(data);
    rv_memory mem = {.first_chunk = &chunk};
)


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
    F(TEST_LB)

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
