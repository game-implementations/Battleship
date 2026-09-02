/*
 * Nintendo Switch implementation of the Platform Abstraction Layer, built on
 * libnx (devkitPro / devkitA64). Compiled only for the Switch target.
 *
 * All PAL entry points are real: console output and lifecycle via the applet
 * main loop, "press any key" via the gamepad, and line input via the system
 * software keyboard (swkbd).
 */
#ifdef __SWITCH__

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

/* Upper bound on a single swkbd entry; matches the desktop line buffer. */
#define PLATFORM_SWKBD_MAX 255

static PadState g_pad;
static bool g_console_up = false;

/* Owned copy of a C string; mirrors the "caller frees" contract of the PAL. */
static char* dup_cstr(const char* s)
{
    size_t n = strlen(s) + 1;
    char* p = (char*) malloc(n);
    if (p != NULL) {
        memcpy(p, s, n);
    }
    return p;
}

void platform_init(void)
{
    consoleInit(NULL);
    g_console_up = true;

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&g_pad);
}

void platform_shutdown(void)
{
    if (!g_console_up) {
        return;
    }
    /* Hold the final screen so the user can read it, then leave on +. */
    while (appletMainLoop()) {
        padUpdate(&g_pad);
        if (padGetButtonsDown(&g_pad) & HidNpadButton_Plus) {
            break;
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    g_console_up = false;
}

bool platform_should_run(void)
{
    return appletMainLoop();
}

void platform_frame_end(void)
{
    consoleUpdate(NULL);
}

void platform_wait_any_key(void)
{
    padUpdate(&g_pad); /* clear whatever is held right now */
    while (appletMainLoop()) {
        padUpdate(&g_pad);
        if (padGetButtonsDown(&g_pad) != 0) {
            break;
        }
        consoleUpdate(NULL);
    }
}

char* platform_read_line(void)
{
    /*
     * Pop the system software keyboard. On any failure or a user cancel we
     * return an empty string: every caller in libinput re-prompts on invalid
     * input, which simply re-opens the keyboard.
     */
    SwkbdConfig kbd;
    if (R_FAILED(swkbdCreate(&kbd, 0))) {
        return dup_cstr("");
    }
    swkbdConfigMakePresetDefault(&kbd);
    swkbdConfigSetStringLenMax(&kbd, PLATFORM_SWKBD_MAX);

    char buf[PLATFORM_SWKBD_MAX + 1];
    buf[0] = '\0';
    Result rc = swkbdShow(&kbd, buf, sizeof buf);
    swkbdClose(&kbd);

    /* Redraw the text console the keyboard applet drew over. */
    consoleUpdate(NULL);

    if (R_FAILED(rc)) {
        return dup_cstr("");
    }
    return dup_cstr(buf);
}

const char* platform_save_dir(void)
{
    return "sdmc:/switch/battleship/";
}

int platform_menu_select(const char* title, const char* const* labels,
                         const int* values, int count)
{
    int cur = 0;

    padUpdate(&g_pad); /* drop a button still held from the previous screen */

    while (appletMainLoop()) {
        consoleClear();
        if (title != NULL) {
            printf("%s\n\n", title);
        }
        for (int i = 0; i < count; i++) {
            if (i == cur) {
                printf("\x1b[7m> %s\x1b[0m\n", labels[i]); /* reverse video */
            } else {
                printf("  %s\n", labels[i]);
            }
        }
        printf("\nUp / Down to move, A to select\n");
        consoleUpdate(NULL);

        padUpdate(&g_pad);
        u64 down = padGetButtonsDown(&g_pad);
        if (down & HidNpadButton_AnyUp) {
            cur = (cur - 1 + count) % count;
        }
        if (down & HidNpadButton_AnyDown) {
            cur = (cur + 1) % count;
        }
        if (down & HidNpadButton_A) {
            return values[cur];
        }
    }

    /* System asked the app to quit: act as if the last entry (Quit) was picked. */
    return values[count - 1];
}

#endif /* __SWITCH__ */
