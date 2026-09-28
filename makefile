# =============================================================================
#  Battleship - top-level build
#
#  One makefile, two targets, selected with PLATFORM:
#
#      make                  # PLATFORM=posix  -> bin/Battleship      (x86/64)
#      make PLATFORM=switch  # PLATFORM=switch -> bin/Battleship.nro   (homebrew)
#
#  The per-library makefiles under src/lib*/ take CC and CFLAGS as overridable
#  defaults (`?=`). This makefile exports the values chosen below so every
#  nested `make` compiles with the right toolchain and flags.
# =============================================================================

# ---- Directories -----------------------------------------------------------
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src

# ---- Platform selection -----------------------------------------------------
PLATFORM ?= posix

ifeq ($(PLATFORM),posix)
    # Desktop terminals (Linux / macOS / *BSD) on x86/64.
    # -O3, all + extra warnings, debug symbols, link math + libc, PIC, C99.
    CC           ?= gcc
    CFLAGS       ?= -O3 -Wall -Wextra -g -lm -fPIC -std=c99 -lc
    PLATFORM_SRC := platform_posix.c
    ARTIFACT     := $(BIN_DIR)/Battleship

else ifeq ($(PLATFORM),switch)
    # Nintendo Switch homebrew via devkitPro (devkitA64 + libnx).
    # Needs the devkitPro environment: DEVKITPRO pointing at the install
    # (e.g. /opt/devkitpro). If you don't have it locally, use `make switch-docker`.
    # gnu11 (not c99) because <switch.h> relies on C11 anonymous unions.
    ifeq ($(strip $(DEVKITPRO)),)
        $(error PLATFORM=switch needs the devkitPro environment: set DEVKITPRO (e.g. export DEVKITPRO=/opt/devkitpro), or run `make switch-docker`)
    endif
    DEVKITA64    ?= $(DEVKITPRO)/devkitA64
    LIBNX        ?= $(DEVKITPRO)/libnx
    CC           := $(DEVKITA64)/bin/aarch64-none-elf-gcc
    NACPTOOL     := $(DEVKITPRO)/tools/bin/nacptool
    ELF2NRO      := $(DEVKITPRO)/tools/bin/elf2nro
    SWITCH_ARCH  := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
    CFLAGS       ?= -O3 -Wall -Wextra -g -std=gnu11 $(SWITCH_ARCH) -D__SWITCH__ -I$(LIBNX)/include
    SWITCH_LDFLAGS := -specs=$(LIBNX)/switch.specs $(SWITCH_ARCH) -u printf_float -L$(LIBNX)/lib -lnx -lm
    PLATFORM_SRC := platform_switch.c
    ARTIFACT     := $(BIN_DIR)/Battleship.nro

    # .nro metadata shown in the Homebrew Menu (override on the command line).
    APP_TITLE    ?= Battleship
    APP_AUTHOR   ?= AleixMT
    APP_VERSION  ?= 0.1.0
    APP_ICON     ?= assets/icon.jpg

else
    $(error Unsupported PLATFORM "$(PLATFORM)". Use "posix" or "switch".)
endif

# Nested makefiles inherit these instead of their built-in defaults.
export CC
export CFLAGS

# Compiler flags for shared libs (unused by the current static build, kept for
# reference). -shared so a lib without main() still links.
CFLAGS_LIB = -shared #-Wl,-soname,

# ---- Component libraries --------------------------------------------------
BATTLESHIP_LIB=$(LIB_DIR)/libbattleship.a
BATTLESHIP_LIB_DIR=src/libbattleship
INPUT_LIB=$(LIB_DIR)/libinput.a
INPUT_LIB_DIR=src/libinput
DOUBLELINKEDLIST_LIB=$(LIB_DIR)/libdoublelinkedlist.a
DOUBLELINKEDLIST_LIB_DIR=src/libdoublelinkedlist
PLATFORM_LIB=$(LIB_DIR)/libplatform.a
PLATFORM_LIB_DIR=src/libplatform

OBJS = $(OBJ_DIR)/main.o $(BATTLESHIP_LIB) $(INPUT_LIB) $(DOUBLELINKEDLIST_LIB) $(PLATFORM_LIB)

# ---- Targets -------------------------------------------------------------
all : $(ARTIFACT)

# Input library
$(INPUT_LIB) : $(INPUT_LIB_DIR)/libinput.c $(INPUT_LIB_DIR)/libinput.h
	cd $(INPUT_LIB_DIR); $(MAKE)

# Battleship library
$(BATTLESHIP_LIB) : $(BATTLESHIP_LIB_DIR)/libbattleship.c $(BATTLESHIP_LIB_DIR)/libbattleship.h
	cd $(BATTLESHIP_LIB_DIR); $(MAKE)

# Double Linked List library
$(DOUBLELINKEDLIST_LIB) : $(DOUBLELINKEDLIST_LIB_DIR)/libdoublelinkedlist.c $(DOUBLELINKEDLIST_LIB_DIR)/libdoublelinkedlist.h
	cd $(DOUBLELINKEDLIST_LIB_DIR); $(MAKE)

# Platform Abstraction Layer library (one source file chosen per PLATFORM)
$(PLATFORM_LIB) : $(PLATFORM_LIB_DIR)/$(PLATFORM_SRC) $(PLATFORM_LIB_DIR)/platform.h
	cd $(PLATFORM_LIB_DIR); $(MAKE) PLATFORM_SRC=$(PLATFORM_SRC)

$(OBJ_DIR)/main.o : $(SRC_DIR)/main.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/main.o -c $(SRC_DIR)/main.c

# ---- Final artifact (per platform) -------------------------------------
ifeq ($(PLATFORM),posix)

$(BIN_DIR)/Battleship : $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -static -o $(BIN_DIR)/Battleship $(OBJS)
	chmod 111 $(BIN_DIR)/Battleship

else ifeq ($(PLATFORM),switch)

# Link an ELF, build the control.nacp, then wrap both plus the icon as an .nro
# for the Homebrew Menu.
$(BIN_DIR)/Battleship.nro : $(OBJS) $(APP_ICON)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/Battleship.elf $(OBJS) $(SWITCH_LDFLAGS)
	$(NACPTOOL) --create "$(APP_TITLE)" "$(APP_AUTHOR)" "$(APP_VERSION)" $(BIN_DIR)/Battleship.nacp
	$(ELF2NRO) $(BIN_DIR)/Battleship.elf $(BIN_DIR)/Battleship.nro --nacp=$(BIN_DIR)/Battleship.nacp --icon=$(APP_ICON)

endif

# ---- Convenience --------------------------------------------------------
# Run Battleship binary (posix only)
run : $(BIN_DIR)/Battleship
	$(BIN_DIR)/Battleship

# Debug Battleship binary (posix only)
debug : $(BIN_DIR)/Battleship
	gdb $(BIN_DIR)/Battleship

# Build the Switch .nro inside the official devkitPro container, so no local
# toolchain install is needed. Output lands in bin/ owned by the current user.
DOCKER_DKP_IMAGE ?= devkitpro/devkita64:latest
switch-docker :
	docker run --rm -t -u $$(id -u):$$(id -g) \
		-v "$(CURDIR)":/project -w /project \
		$(DOCKER_DKP_IMAGE) make PLATFORM=switch

# ---- Release packaging ------------------------------------------------
# `make dist` stages the current platform's release asset under dist/.
# CI (.github/workflows/release.yml) runs it once per platform on a v* tag.
DIST_DIR = dist

ifeq ($(PLATFORM),posix)
dist : $(BIN_DIR)/Battleship
	mkdir -p $(DIST_DIR)
	chmod 0755 $(BIN_DIR)/Battleship   # `all` sets 0111 (exec-only); tar needs to read it
	tar -czf $(DIST_DIR)/Battleship-linux-$$(uname -m).tar.gz -C $(BIN_DIR) Battleship
	@echo "staged $(DIST_DIR)/Battleship-linux-$$(uname -m).tar.gz"
else ifeq ($(PLATFORM),switch)
dist : $(BIN_DIR)/Battleship.nro
	mkdir -p $(DIST_DIR)
	cp $(BIN_DIR)/Battleship.nro $(DIST_DIR)/Battleship.nro
	@echo "staged $(DIST_DIR)/Battleship.nro"
endif

# Clean compilation objects
.PHONY : all run debug switch-docker dist clean
clean :
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(DIST_DIR)
