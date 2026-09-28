# Submitting Battleship to the Homebrew App Store

The [Homebrew App Store](https://www.fortheusers.org/) (hb-appstore) is the
in-app catalogue most Switch CFW users browse. Listing there is a **manual,
one-time pull request** against the community package repo; it cannot be done
from this repo's CI because it needs a published release URL and a review.

The project is at `0.1.0` — playable but not finished (no save/load/highscore
persistence, input is swkbd-typed rather than fully console-native). Don't
submit to the App Store until a `1.0.0`-ish release; a tagged `0.1.0` build is
fine for early testers grabbing the `.nro` directly off GitHub Releases.

## Prerequisites

- A published GitHub Release with `Battleship.nro` attached — the `Release`
  workflow (`.github/workflows/release.yml`) produces this on a `v*` tag.
- A 256×256 icon and (optionally) 1–3 screenshots. `assets/icon.jpg` is a
  placeholder; replace it with real art before submitting.

## Steps

1. Fork `fortheusers/hb-appstore-repo` on GitHub.
2. Add `packages/Battleship/pkgbuild.json` using `pkgbuild.template.json` in this
   directory as a starting point. Fill in:
   - `assets[].url` → the release asset URL, e.g.
     `https://github.com/game-implementations/Battleship/releases/download/v0.1.0/Battleship.nro`
   - `info.version` → the release tag without the leading `v`
   - `info.license`, `info.description`, `info.details`
3. Drop `icon.png` (256×256) and `screen1.png` … in the same folder.
4. Validate against their current `CONTRIBUTING.md` / schema (the exact keys have
   changed over time — treat the template as a hint, not gospel).
5. Open the PR. Their CI checks the manifest and a maintainer merges it; the app
   appears in the store shortly after.

## Updating

Bump `info.version` and the asset `url` in the same `pkgbuild.json` and open a new
PR for each release.
