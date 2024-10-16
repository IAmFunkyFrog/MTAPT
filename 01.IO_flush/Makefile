CC=gcc
RM=rm -rf
BUILD_DIR=build
SRC=src

all: main

build_dir:
	mkdir -p build

main: $(SRC)/main.c build_dir
	$(CC) $(SRC)/main.c -o $(BUILD_DIR)/experiment

clean:
	$(RM) $(BUILD_DIR)
