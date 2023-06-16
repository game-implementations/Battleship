# Compiler
CC = gcc
# Compiler flags:
# Optimization level = 3, show warnings, show extra warnings, compile with debugger symbols, link math library, Used to
# link libraries, C standard = 1999, link against libc
CFLAGS = -O3 -Wall -Wextra -g -lm -fPIC -std=c99 -lc
# Compiler flags for libs:
# Marks the output as shared library so we do not fail without a main, -Wl set of parameters separated by ','. The only
# parameter is -soname, which allows to mark the library with a SO_tag
CFLAGS_LIB = -shared #-Wl,-soname,

# change these to proper directories where each file should be
OBJ_DIR = ../obj
BIN_DIR = ../bin
LIB_DIR = ../lib

# Path to the libraries used
BATTLESHIP_LIB=$(LIB_DIR)/libbattleship.a
BATTLESHIP_LIB_DIR=libbattleship
INPUT_LIB=$(LIB_DIR)/libinput.a
INPUT_LIB_DIR=libinput

# Main target
all : $(BIN_DIR)/Battleship

# Input library
$(INPUT_LIB) : $(INPUT_LIB_DIR)/libinput.c $(INPUT_LIB_DIR)/libinput.h
	cd $(INPUT_LIB_DIR); make

# Battleship library
$(BATTLESHIP_LIB) : $(BATTLESHIP_LIB_DIR)/libbattleship.c $(BATTLESHIP_LIB_DIR)/libbattleship.h
	cd $(BATTLESHIP_LIB_DIR); make

$(OBJ_DIR)/main.o : main.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/main.o -c main.c

# Battleship binary
$(BIN_DIR)/Battleship : $(OBJ_DIR)/main.o $(BATTLESHIP_LIB) $(INPUT_LIB)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -static -o $(BIN_DIR)/Battleship $(OBJ_DIR)/main.o $(INPUT_LIB) $(BATTLESHIP_LIB)
	chmod 111 $(BIN_DIR)/Battleship

# Run Battleship binary
run : $(BIN_DIR)/Battleship
	$(BIN_DIR)/Battleship

# Debug Battleship binary
debug : $(BIN_DIR)/Battleship
	gdb $(BIN_DIR)/Battleship

# Clean compilation objects
.PHONY : clean
clean :
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)
