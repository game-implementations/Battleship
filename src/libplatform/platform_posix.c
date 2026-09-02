/*
 * POSIX / desktop implementation of the Platform Abstraction Layer.
 *
 * Target: any plain terminal on x86/64 (Linux, macOS, *BSD). Input comes from
 * stdin, output goes to stdout, and there is no host lifecycle to obey, so most
 * of these functions are thin wrappers over the C standard library. This file
 * deliberately reproduces the exact stdin behaviour the game had before the PAL
 * was introduced, so the desktop build is unchanged.
 *
 * Compiled only when NOT building for the Switch.
 */
#ifndef __SWITCH__

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Big enough for any single line of interactive input we expect. */
#define PLATFORM_LINE_BUFSIZE 256

/* Drop everything left on the current stdin line, up to and including '\n'. */
static void discard_rest_of_line(void)
{
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF) {}
}

void platform_init(void)
{
    /* Nothing to set up on a terminal. */
}

void platform_shutdown(void)
{
    /* Nothing to tear down on a terminal. */
}

bool platform_should_run(void)
{
    return true;
}

void platform_frame_end(void)
{
    fflush(stdout);
}

void platform_wait_any_key(void)
{
    int c = fgetc(stdin);
    if (c != '\n' && c != EOF) {
        discard_rest_of_line();
    }
}

char* platform_read_line(void)
{
    char buf[PLATFORM_LINE_BUFSIZE];

    for (;;) {
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            /* EOF or error: clear the flag and keep waiting for real input. */
            clearerr(stdin);
            continue;
        }

        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n') {
            /* Line was longer than the buffer: swallow the remainder. */
            discard_rest_of_line();
        } else if (len > 0) {
            buf[len - 1] = '\0'; /* strip the newline */
        }

        char* result = (char*) malloc(strlen(buf) + 1);
        if (result == NULL) {
            continue; /* transient: prompt again rather than crash */
        }
        strcpy(result, buf);
        return result;
    }
}

const char* platform_save_dir(void)
{
    return "./";
}

int platform_menu_select(const char* title, const char* const* labels,
                         const int* values, int count)
{
    (void) title;
    (void) labels;
    (void) values;
    (void) count;
    return PLATFORM_MENU_NOT_HANDLED; /* caller keeps its text prompt */
}

#endif /* !__SWITCH__ */
