CC=gcc
RM=rm -rf
BUILD_DIR=build
SRC=src
TEST=test
INTEGRATION_TESTS=integration_tests

all: main

build_dir:
	mkdir -p $(BUILD_DIR)

build_objects: $(SRC)/*.c build_dir
	mkdir -p $(BUILD_DIR)/lib
	$(CC) -fwrapv -c $(SRC)/interpreter.c -o $(BUILD_DIR)/lib/interpreter.o
	$(CC) -c $(SRC)/memory.c -o $(BUILD_DIR)/lib/memory.o
	$(CC) -c $(SRC)/cpu.c -o $(BUILD_DIR)/lib/cpu.o

main: build_objects
	$(CC) -c $(SRC)/main.c -o $(BUILD_DIR)/main.o
	$(CC) $(BUILD_DIR)/*.o $(BUILD_DIR)/lib/*.o -o $(BUILD_DIR)/simulator

build_test: build_objects
	mkdir -p $(BUILD_DIR)/test
	$(CC) -I$(SRC) $(TEST)/main.c -r -o $(BUILD_DIR)/test/main.o
	$(CC) $(BUILD_DIR)/lib/*.o $(BUILD_DIR)/test/*.o -o $(BUILD_DIR)/test/run_test

test: build_test main
	$(BUILD_DIR)/test/run_test
	chmod +x $(INTEGRATION_TESTS)/*
	$(INTEGRATION_TESTS)/compute_fib_5.sh

clean:
	$(RM) $(BUILD_DIR)
