#ifndef BATTLESHIP_PLATFORM_H
#define BATTLESHIP_PLATFORM_H

#include <stdbool.h>

/*
 * Platform Abstraction Layer (PAL)
 * --------------------------------
 * Everything that differs between the desktop build (x86/64, plain terminal)
 * and a console build (e.g. Nintendo Switch homebrew via libnx) lives behind
 * this interface. The game logic in libbattleship talks only to these
 * functions and to plain printf(); it never touches stdin, termios, libnx or
 * any host API directly.
 *
 * Each supported platform provides exactly one implementation file:
 *   - platform_posix.c   -> desktop terminals (Linux / macOS / *BSD)
 *   - platform_switch.c  -> Nintendo Switch (added in a later phase)
 * The build system compiles the one matching the target and ignores the rest.
 */

/** One-time bring-up: console/framebuffer, input devices, filesystem mounts. */
void platform_init(void);

/** Tear down whatever platform_init() created. Safe to call exactly once. */
void platform_shutdown(void);

/**
 * True while the host still wants the program to run. Always true on desktop;
 * on a console target this follows the system's applet main loop, so callers
 * must use it as their top-level loop condition instead of `while (true)`.
 */
bool platform_should_run(void);

/**
 * Push the text written since the last call to the screen. On a desktop
 * terminal this is just a stdout flush; on a console it swaps the text
 * framebuffer. Call it after drawing a screen and before blocking for input.
 */
void platform_frame_end(void);

/** Block until the user presses any key / button. Replaces pauseExecution(). */
void platform_wait_any_key(void);

/**
 * Read one line of user input with the trailing newline removed. Never returns
 * NULL: it blocks and retries until a line is available. The caller owns the
 * returned buffer and must free() it. An empty line yields a valid "" string.
 */
char* platform_read_line(void);

/** Directory (with trailing '/') where save files and high scores belong. */
const char* platform_save_dir(void);

#endif /* BATTLESHIP_PLATFORM_H */
