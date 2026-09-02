/*
 * Nintendo Switch implementation of the Platform Abstraction Layer, built on
 * libnx (devkitPro / devkitA64). Compiled only for the Switch target.
 *
 * Phase 2 scope: console output, the application lifecycle (applet main loop,
 * screen flushing) and "press any key" are real. Text / number entry
 * (platform_read_line) is a deliberate stub here - it is replaced with the
 * libnx software keyboard in Phase 3 - so for now the menu loop always reaches
 * its clean quit path instead of spinning on input that can never arrive.
 */
#ifdef __SWITCH__

#include "platform.h"

#include <stdlib.h>
#include <string.h>
#include <switch.h>

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
    /* TODO(phase3): swkbdCreate / swkbdShow for real on-screen text entry. */
    return dup_cstr("6");
}

const char* platform_save_dir(void)
{
    return "sdmc:/switch/battleship/";
}

#endif /* __SWITCH__ */
