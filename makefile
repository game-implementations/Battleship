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
SRCDIR = src
OBJDIR = obj
BINDIR = bin
LIBDIR = lib

# Main target
all : $(BINDIR)/Battleship

# Double Linked List object
$(OBJDIR)/input.o : $(SRCDIR)/input.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $(OBJDIR)/input.o -c $(SRCDIR)/input.c

# Double Linked List library
$(LIBDIR)/input.so : $(OBJDIR)/input.o
	mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) $(CFLAGS_LIB)input.so -o $(LIBDIR)/input.so $(OBJDIR)/input.o

# Battleship object
$(OBJDIR)/Battleship.o : $(SRCDIR)/Battleship.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $(OBJDIR)/Battleship.o -c $(SRCDIR)/Battleship.c

# Battleship binary
$(BINDIR)/Battleship : $(OBJDIR)/Battleship.o $(LIBDIR)/input.so
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/Battleship $(OBJDIR)/Battleship.o $(LIBDIR)/input.so
	chmod 111 $(BINDIR)/Battleship

# Run Battleship binary
run : $(BINDIR)/Battleship
	$(BINDIR)/Battleship

# Debug Battleship binary
debug : $(BINDIR)/Battleship
	gdb $(BINDIR)/Battleship

# Clean compilation objects
.PHONY : clean
clean :
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)
