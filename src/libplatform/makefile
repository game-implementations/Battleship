# Compiler
CC ?= gcc
# Compiler flags: see the sibling library makefiles for the rationale.
CFLAGS ?= -O3 -Wall -Wextra -g -lm -fPIC -std=c99 -lc

# change these to proper directories where each file should be
OBJ_DIR = ../../obj
BIN_DIR = ../../bin
LIB_DIR = ../../lib
SRC_DIR = .

# Which implementation of the Platform Abstraction Layer to compile.
# The root makefile overrides this (PLATFORM_SRC=platform_switch.c) for the
# Nintendo Switch build; on its own this makefile targets the desktop.
PLATFORM_SRC = platform_posix.c

# Platform library
$(LIB_DIR)/libplatform.a : $(OBJ_DIR)/libplatform.o
	mkdir -p $(LIB_DIR)
	ar -cvq $(LIB_DIR)/libplatform.a $(OBJ_DIR)/libplatform.o

# Platform object
$(OBJ_DIR)/libplatform.o : $(SRC_DIR)/$(PLATFORM_SRC) $(SRC_DIR)/platform.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/libplatform.o -c $(SRC_DIR)/$(PLATFORM_SRC)

# Clean compilation objects
.PHONY : clean
clean :
	rm -rf $(OBJ_DIR)/libplatform.o $(LIB_DIR)/libplatform.a
