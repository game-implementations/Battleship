# Compiler
CC = gcc
# Compiler flags:
# Optimization level = 3, show warnings, show extra warnings, compile with debugger symbols, link math library, Used to
# link libraries, C standard = 1999, link against libc
CFLAGS = -O3 -Wall -Wextra -g -lm -fPIC -std=c99 -lc
# Compiler flags for libs:
# Marks the output as shared library so we do not fail without a main, -Wl set of parameters separated by ','. The only
# parameter is -soname, which allows to mark the library with a SO_tag
CFLAGS_LIB = -shared -Wl,-soname,

# change these to proper directories where each file should be
OBJ_DIR = ../../obj
BIN_DIR = ../../bin
LIB_DIR = ../../lib
SRC_DIR = .

# double linked list library
$(LIB_DIR)/libdoublelinkedlist.a : $(OBJ_DIR)/libdoublelinkedlist.o
	mkdir -p $(LIB_DIR)
	ar -cvq $(LIB_DIR)/libdoublelinkedlist.a $(OBJ_DIR)/libdoublelinkedlist.o

# double linked list object
$(OBJ_DIR)/libdoublelinkedlist.o : $(SRC_DIR)/libdoublelinkedlist.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/libdoublelinkedlist.o -c $(SRC_DIR)/libdoublelinkedlist.c

# Clean compilation objects
.PHONY : clean
clean :
	rm -rf $(OBJ_DIR)/libdoublelinkedlist.o $(LIB_DIR)/libdoublelinkedlist.a
