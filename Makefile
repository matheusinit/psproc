CC = cc
CFLAGS =-W -Wall
CFLAGS +=-std=c99
BUILD_DIR = ./build/
SRC=./src/*.c

.PHONY: all
all: compile

.PHONY: compile
compile:
	mkdir -p $(BUILD_DIR) && $(CC) $(CFLAGS) -o build/proc $(SRC)

.PHONY: run
run:
	(cd $(BUILD_DIR) && ./proc)

.PHONY: check
check:
	(cd $(BUILD_DIR) && valgrind --leak-check=full ./proc)
