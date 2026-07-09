# Track: Build speed (C++ backend + link)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.9; baseline: mlcc codegen ~2s, g++ link 90�200s

## Status: **closed** (STEP=7 done)

**Depends on:** none (infra-only; parallel with other tracks).

**Baseline:** `build_bin.sh` � parallel compile, `ccache clang++` preferred, `lld`; obj in `mktemp` (no reuse); `build.sh` wipes `out/*.cpp` each rebuild; 132 generated TU.

**STEP=1 note (2026-05-19):** persistent `$CPP_DIR/obj/`; mtime skip; `MLCC_OBJ_CLEAN=1`; exclude `tests_main.cpp`. Warm `link_sec≈0.67s`. Gate **1290/0**.

**STEP=2 note (2026-05-19):** `MLCC_DEV=1` → `-O0 -g` (`obj/dev/`); `MLCC_OPT` default `2` → `-O2` (`obj/O2/`). Gate **1290/0**; release warm `link_sec≈1.5s`.

**STEP=3 note (2026-06-25):** linker mold → lld → gold in `build_bin.sh`; README. Warm `link_sec≈1.98s`; gate **1297/0**.

**STEP=4 note (2026-06-25):** `mlcc_precompiled.hpp` PCH in `build_bin.sh` (`MLCC_PCH=0` off); clang `-include-pch`, g++ `.gch`. `decls.cpp`: 25.7s → 20.1s; pch build 7s. Gate **1297/0**.

**STEP=5 note (2026-06-25):** `MLCC_INCREMENTAL=1` + `.mlcc_module_stamp` (module list fingerprint + cpp manifest); skip `find -delete` when stamp matches. Warm `link_sec=0.02s`. Gate **1297/0**.

**STEP=6 note (2026-06-25):** CI `ci.yml` + `triple-bootstrap.yml`: apt `clang ccache mold g++`; job `MLC_CXX=ccache clang++`. Gate **1297/0**.

**STEP=7 note (2026-06-25):** `compiler/scripts/bench_build.sh` (warm skip + link); track **closed**. `build_sh_skip=0.04s`; gate **1297/0**.

**Correction (2026-07-01):** STEP=5's `.mlcc_module_stamp` fingerprint hashes the **list of `.mlc` filenames only**, not content — content-only edits to an existing `.mlc` file leave the fingerprint unchanged, so `should_skip_codegen()` returns true and mlcc codegen is silently skipped, keeping stale `compiler/out/*.cpp`. Live-reproduced 2026-07-01 (see [TRACK_BUILD_SPEED2.md](TRACK_BUILD_SPEED2.md)). Also: `build_bin.sh`'s object-staleness check (`.cpp` mtime vs `.o` mtime) has no header-dependency tracking. Cold rebuild measured **564.9s** (147 TU); `ccache` + clang `-include-pch` gives **0 cache hits** (all uncacheable). Follow-up: [TRACK_BUILD_SPEED2.md](TRACK_BUILD_SPEED2.md).

**Correction fixed (2026-07-03):** [TRACK_BUILD_SPEED2.md](TRACK_BUILD_SPEED2.md) STEP=1 (content-based fingerprint), STEP=2 (depfile-based staleness), STEP=3 (ccache PCH sloppiness) all done — the three gaps above are closed.

**Goal:** dev rebuild ? 30s; CI link acceptable. mlcc codegen already fast � optimize C++ compile/link + incremental artifacts.

## Verify gate (every step)

```
# cold-ish rebuild after compiler/ touch
/usr/bin/time -f 'link_sec=%e' compiler/build.sh
compiler/tests/build_tests.sh
```

Record `link_sec` in SESSION; must not regress vs baseline after each step.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Persistent `out/obj/` in `build_bin.sh` (drop `mktemp`); ccache-friendly | done |
| 2 | `MLCC_DEV=1` → `-O0 -g`; `MLCC_OPT=2` default `-O2` for release | done |
| 3 | Linker: prefer `mold`, then `lld`, then gold; document in README | done |
| 4 | PCH or bundled include for hot headers (`mlc.hpp`, json) � measure before/after | done |
| 5 | `build.sh`: optional skip `find -delete out/*.cpp` when `MLCC_INCREMENTAL=1` + module stamp | done |
| 6 | CI: install `clang`, `ccache`, `mold`; `MLC_CXX="ccache clang++"` | done |
| 7 | Benchmark script `compiler/scripts/bench_build.sh`; close track | done |

### Clang � ????? ???

| | clang++ | g++ |
|--|---------|-----|
| **??????????** | ??? | fallback ? `build_bin.sh` |
| **????????????** | ?? | � |
| **??????** | ??????? compile ?? ??????? TU, ????? diagnostics, `-fuse-ld=lld/mold` | ?????? ???? ? CI |

Default chain (already): `MLC_CXX` ? `ccache clang++` ? `clang++` ? `ccache g++` ? `g++`.

**Dev setup:** `apt install clang ccache mold` (or `MLC_CXX='ccache clang++'`).

### Out of scope

- LLVM/Cranelift native backend (separate future track)
- Incremental mlcc modular codegen (architecture track / later)
- Unity-build amalgamation (STEP=5 alternative if PCH insufficient)

---

## Per-turn template

```
| step | <1�7> |
| done | <one line> |
| verify | link_sec=�; build_tests N/0 |
```
