# Battleship

A C implementation of the classic Battleship game, played in a text console.
Human vs human, human vs machine, or machine vs machine on a square board
(dimensions 8–23). The machine player uses a small state machine to hunt and
sink ships in as few shots as possible.

## Core ideas

- **Language / standard:** C99, warnings-as-guidance (`-O3 -Wall -Wextra -std=c99`).
  Standard library only — no ncurses, SDL, threads, or sockets.
- **Two boards per player:** an *attack* board (what you've discovered about the
  enemy) and a *defense* board (your own ships). Cell glyphs and shot results are
  `#define`d at the top of `libbattleship.h` (`?` undiscovered, `~` water,
  `-` shot water, `@` ship, `X` shot ship).
- **Ships** are placed automatically (random fit, retried until the board hits
  `BOARD_USAGE_PERCENTAGE`) or manually, ship by ship, from user input.
- **Menu-driven main loop** in `src/main.c`: create / load / play / save /
  highscore / quit. Load, save and highscore persistence are **not implemented
  yet** (stubs in `libbattleship.c` and `main.c`).
- **Scoring** rewards larger boards and fewer total shots.

## Layout

| Path | Role |
| --- | --- |
| `src/main.c` | Entry point and menu loop |
| `src/libbattleship/` | All game rules, board rendering, AI, menu screens |
| `src/libinput/` | Validated console input (`readInt`, `readChar`, `readString`, ranges/sets) |
| `src/libdoublelinkedlist/` | Generic doubly linked list, used for high-score records |
| `src/libplatform/` | **Platform Abstraction Layer (PAL)** — see below |
| `doc/`, `README.md` | Original assignment spec and function notes |

Each `src/lib*/` builds a static archive via its own nested `makefile`; the root
`makefile` links them into `bin/Battleship`. Build objects land in `obj/`,
archives in `lib/`, the binary in `bin/` (all git-ignored).

## Portability: desktop + Nintendo Switch

Goal: keep running on x86/64 desktop terminals **and** run as Nintendo Switch
homebrew (a `.nro` built with devkitPro / libnx), from one source tree.

Everything host-specific lives behind **`src/libplatform/platform.h`**. Game code
calls only that interface plus plain `printf()`; it never touches `stdin`,
`termios`, or any host API. One implementation file is compiled per target:

- `platform_posix.c` — desktop terminals (Linux / macOS / *BSD). Compiled unless
  `__SWITCH__` is defined. Reproduces the exact pre-PAL stdin behaviour.
- `platform_switch.c` — Nintendo Switch via libnx *(added in a later phase)*.

PAL surface: `platform_init` / `platform_shutdown`, `platform_should_run` (loop
condition — never `while (true)`), `platform_frame_end` (flush a screen of text),
`platform_wait_any_key` (replaces the old `pauseExecution`), `platform_read_line`
(owned, newline-stripped, never NULL), `platform_save_dir`.

`libinput`'s readers are now thin validators on top of `platform_read_line()`.
There is deliberately **no** in-tree `memcpy` — the previous custom definition
shadowed libc and was removed.

### Build

```sh
make                  # desktop build -> bin/Battleship       (PLATFORM=posix, default)
make PLATFORM=switch  # homebrew build -> bin/Battleship.nro   (needs $DEVKITPRO)
make clean
```

**One makefile, conditional on `PLATFORM`** — no separate `Makefile.switch` and
no devkitPro `switch_rules` include; the `.nro` packaging (`elf2nro`) is spelled
out explicitly, consistent with the project's hand-rolled build. The root
`makefile` picks `CC` / `CFLAGS` / `PLATFORM_SRC` / final-artifact recipe per
platform and `export`s `CC` and `CFLAGS`. The four `src/lib*/makefile` files
declare `CC ?=` / `CFLAGS ?=` so they use those exported values, and still build
standalone with desktop defaults if invoked directly. `posix` is fully working;
the `switch` branch is written but unverified until the Phase 2 toolchain lands.

### Porting roadmap

1. **PAL + POSIX impl, desktop unchanged; one-makefile build wiring.** ✅ done
2. devkitA64 toolchain + `platform_switch.c`; verify the `switch` makefile branch
   links a `.nro` that shows the menu.
3. Switch input: libnx software keyboard for text/number entry, `PadState` for
   "press any key" and menu nav; applet main-loop + suspend/resume; `.nro`
   metadata + icon.
4. Implement save / load / highscore persistence via `platform_save_dir()`
   (`./` on desktop, `sdmc:/switch/battleship/` on Switch).
5. Package: CI matrix (desktop smoke build + containerized `.nro`), GitHub
   Releases, optional Homebrew App Store submission.

"Publish" means homebrew distribution — the eShop requires a licensed Nintendo
SDK that is not available here.

## Conventions

- camelCase functions and variables; `UPPER_SNAKE` macros; brace on its own line.
- Doc comments on declarations in the `.h`, not the `.c`.
- Keep desktop behaviour byte-for-byte stable when refactoring: build a baseline
  from `HEAD` in a `git worktree` and diff program output for several scripted
  input sequences (menu nav, invalid-input retries, a full machine-vs-machine
  game, a human game that pauses back to menu).
- Pre-existing warnings not to chase in unrelated work: `records` unused in
  `main.c`; use-after-free in `libdoublelinkedlist.c:105` (`destroy`).

## Branch

Switch-portability work is on the `nswitch` branch. `master` is the desktop
baseline; `main` tracks `origin/main`.
