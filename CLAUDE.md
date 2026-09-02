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
| `assets/` | `icon.jpg` — 256×256 placeholder icon embedded in the `.nro` |
| `.github/workflows/` | `ci.yml` (build + smoke both targets), `release.yml` (`v*` tag → GitHub Release) |
| `packaging/` | Homebrew App Store submission notes + `pkgbuild` template |
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
- `platform_switch.c` — Nintendo Switch via libnx, all entry points real:
  `consoleInit`/`Exit`, `appletMainLoop` (drives `platform_should_run`),
  `consoleUpdate` (`platform_frame_end`), `PadState` for `platform_wait_any_key`,
  the system software keyboard (`swkbdCreate`/`swkbdShow`, default preset,
  255-char cap) for `platform_read_line` (a cancel returns `""`, which every
  `libinput` reader treats as invalid and re-prompts), and a `consoleClear`
  redraw loop for `platform_menu_select` (D-pad / stick to move, A to select).

PAL surface: `platform_init` / `platform_shutdown`, `platform_should_run` (loop
condition — never `while (true)`), `platform_frame_end` (flush a screen of text),
`platform_wait_any_key` (replaces the old `pauseExecution`), `platform_read_line`
(owned, newline-stripped, never NULL), `platform_save_dir`, `platform_menu_select`
(D-pad menu; returns `PLATFORM_MENU_NOT_HANDLED` on desktop so the caller keeps
its numeric text prompt — `readMenuEntry` is the only caller).

`libinput`'s readers are now thin validators on top of `platform_read_line()`.
There is deliberately **no** in-tree `memcpy` — the previous custom definition
shadowed libc and was removed.

### Build

```sh
make                       # desktop build  -> bin/Battleship      (PLATFORM=posix, default)
make PLATFORM=switch       # homebrew build -> bin/Battleship.nro   (needs $DEVKITPRO set)
make switch-docker         # same, inside the devkitpro/devkita64 container (no local toolchain)
make dist                  # stage the current platform's release asset under dist/
make PLATFORM=switch dist  #   -> dist/Battleship.nro
make clean
```

**One makefile, conditional on `PLATFORM`** — no separate `Makefile.switch` and
no devkitPro `switch_rules` include; the `.nro` packaging (`elf2nro`) is spelled
out explicitly, consistent with the project's hand-rolled build. The root
`makefile` picks `CC` / `CFLAGS` / `PLATFORM_SRC` / final-artifact recipe per
platform and `export`s `CC` and `CFLAGS`. The four `src/lib*/makefile` files
declare `CC ?=` / `CFLAGS ?=` so they use those exported values, and still build
standalone with desktop defaults if invoked directly.

The `switch` branch uses `aarch64-none-elf-gcc`, `-std=gnu11` (`<switch.h>` needs
C11 anonymous unions), the cortex-a57 arch flags, `-D__SWITCH__`,
`-specs=…/switch.specs -lnx`, `-u printf_float` (newlib omits `%f` otherwise),
then `nacptool` + `elf2nro`. Output: a static-PIE aarch64 ELF wrapped as
`bin/Battleship.nro` (`NRO0` header, `ASET` segment with the JPEG icon and a
control.nacp). Homebrew-Menu metadata is overridable: `APP_TITLE`, `APP_AUTHOR`,
`APP_VERSION`, `APP_ICON` (default `assets/icon.jpg`, a 256×256 placeholder).
Builds clean via Docker; **not yet run on hardware/emulator**.

### Porting roadmap

1. **PAL + POSIX impl, desktop unchanged; one-makefile build wiring.** ✅ done
2. **devkitA64 toolchain + `platform_switch.c`; `switch` branch links a valid
   `.nro`.** ✅ done (build verified via Docker; on-hardware menu check still open)
3. Switch input + packaging.
   - ✅ swkbd `platform_read_line`; `.nro` metadata (`nacptool`) + icon;
     D-pad/A menu navigation (`platform_menu_select`, used by `readMenuEntry`).
   - Open, needs on-device iteration: B → `@`/`0` back-to-menu sentinels during a
     turn (coordinates are still swkbd-typed); explicit applet suspend/resume
     redraw (libnx defaults cover the basics); real 256×256 icon to replace the
     placeholder; the on-hardware/emulator smoke test.
4. **Skipped for now.** Save / load / highscore persistence via
   `platform_save_dir()` (`./` desktop, `sdmc:/switch/battleship/` Switch) — the
   game's load/save are still stubs.
5. Packaging + release. ✅ done
   - `.github/workflows/ci.yml` — every push/PR: desktop `make` + smoke test,
     and `make PLATFORM=switch` in the `devkitpro/devkita64` container with an
     NRO0/ASET check, uploading `Battleship.nro`.
   - `.github/workflows/release.yml` — on a `v*` tag: `make dist` per platform,
     then `gh release create` with the `.tar.gz` and the `.nro` (the `.nro` is
     stamped with the tag via `APP_VERSION`). Needs `permissions: contents:write`.
   - `make dist` stages `dist/Battleship-linux-<arch>.tar.gz` (posix) or
     `dist/Battleship.nro` (switch).
   - Homebrew App Store listing is a manual PR — see `packaging/hb-appstore.md`
     and `packaging/pkgbuild.template.json`.

"Publish" means homebrew distribution — the eShop requires a licensed Nintendo
SDK that is not available here.

## CI / release

- Cut a release: `git tag v1.2.3 && git push origin v1.2.3` → the `Release`
  workflow builds both targets and publishes a GitHub Release with auto notes.
- The desktop `all` recipe leaves `bin/Battleship` mode `0111` (exec-only); the
  posix `dist` recipe `chmod 0755`s it first so `tar` can read it.
- No local Actions runner here — workflow steps were validated by running their
  exact commands (`make`, the smoke pipe, `make PLATFORM=switch dist`) in the
  container.

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
