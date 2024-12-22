#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "interpreter.h"
#include "assert.h"

#define TEST_ONE_INSTR_START_PC(name, instr_encoding, condition, start_pc) \
    ExitCode name() { \
        rv_instruction instr = (rv_instruction) {.encoding = instr_encoding}; \
        rv_cpu cpu = {.pc = start_pc}; \
        ExitCode code = rv_cpu_cycle(&cpu, instr); \
        if (code != OK) {\
            rv_dump_cpu(&cpu); \
            return code; \
        } \
        if (condition) \
            return OK; \
        rv_dump_cpu(&cpu); \
        return ERROR; \
    }

#define TEST_ONE_INSTR(name, instr_encoding, condition) \
    TEST_ONE_INSTR_START_PC(name, instr_encoding, condition, 0)

// addi x1, x0, 1
TEST_ONE_INSTR(TEST_ADDI_POSITIVE_IMM, 0x00100093, cpu.x[1] == 1)
// addi x1, x0, -1
TEST_ONE_INSTR(TEST_ADDI_NEGATIVE_IMM, 0xfff00093, cpu.x[1] == -1)
// lui x1, 1
TEST_ONE_INSTR(TEST_LUI, 0x000010b7, cpu.x[1] == 0x00001000)
// auipc x1, 1
TEST_ONE_INSTR_START_PC(TEST_AUIPC, 0x00001097, cpu.x[1] == 0x00001004, 4)
// jal x1, -4
TEST_ONE_INSTR_START_PC(TEST_JAL_NEGATIVE_IMM, 0xffdff0ef, cpu.pc == 0 && cpu.x[1] == 8, 4)
// jal x1, 8
TEST_ONE_INSTR_START_PC(TEST_JAL_POSITIVE_IMM, 0x008000ef, cpu.pc == 12 && cpu.x[1] == 8, 4)


#define TEST_FUNCTIONS(F) \
    F(TEST_ADDI_POSITIVE_IMM) \
    F(TEST_ADDI_NEGATIVE_IMM) \
    F(TEST_LUI) \
    F(TEST_AUIPC) \
    F(TEST_JAL_NEGATIVE_IMM) \
    F(TEST_JAL_POSITIVE_IMM)

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
