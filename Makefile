CC = cc
CFLAGS =-W -Wall
CFLAGS +=-std=c99
BUILD_DIR = ./build/

.PHONY: all
all: compile

compile:
	mkdir -p $(BUILD_DIR) && $(CC) $(CFLAGS) -o build/proc main.c
run:
	(cd $(BUILD_DIR) && ./proc)
