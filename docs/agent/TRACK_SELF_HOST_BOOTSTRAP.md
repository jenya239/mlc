# Track: Self-host bootstrap (remove Ruby from build.sh)

Parent: [../PLAN.md](../PLAN.md) §Phase 4; follows [TRACK_PHASE26_REMAINING.md](TRACK_PHASE26_REMAINING.md)

## Status: **closed** (2026-05-19)

**Depends on:** TRACK_PHASE26_REMAINING closed.

**Goal:** `compiler/build.sh` produces `mlcc` via mlcc-only path; Ruby retained only for `rake test_mlc` reference. **Achieved.**

## Verify gate

```
compiler/tests/build_tests.sh
bash compiler/tests/run_mlcc2_checker_parity.sh
MLCC_BOOTSTRAP=1 compiler/build.sh
bash compiler/tests/run_mlcc_bootstrap_parity.sh
```

Cold start: `MLCC_FORCE_RUBY=1 compiler/build.sh` (CI, bisect, `rake triple_bootstrap`).

---

| Step | Item | Status |
|------|------|--------|
| 1 | Document current bootstrap path; inventory Ruby-only steps in `build.sh` | done |
| 2 | `build.sh` — default path uses existing `compiler/out/mlcc` when fresh enough | done |
| 3 | `MLCC_BOOTSTRAP=1` — chain mlcc → g++ → mlcc_bootstrap; CI smoke | done |
| 4 | Parity: mlcc_bootstrap output ≡ mlcc (diff self-host) | done |
| 5 | Remove Ruby from default `build.sh`; gate; close track | done |

**STEP=5 note (2026-05-19 Driver):** default `build.sh` mlcc-only; cold start `MLCC_FORCE_RUBY=1`; CI/triple-bootstrap/Rake/bisect updated. Gate: parity **2/2**, bootstrap **ok**, bootstrap parity **diff_exit=0**.

**STEP=2 note (2026-05-19 Driver):** `mlcc_binary_is_fresh` (mtime vs `compiler/**/*.mlc`); skip / `build_via_mlcc` / `build_via_ruby`; `MLCC_FORCE_RUBY=1`. Gate: self-host **diff_exit=0**.

**STEP=3 note (2026-05-19 Driver):** bootstrap — `mlcc -o bootstrap/` + `build_bin.sh` → `out/bootstrap/mlcc_bootstrap`; hard fail on error; stale mlcc + `MLCC_BOOTSTRAP=1` skips main self-rebuild; CI smoke in `ci.yml`. Gate: bootstrap **ok**; parity **2/2**.

**STEP=4 note (2026-05-19 Driver):** `compiler/tests/run_mlcc_bootstrap_parity.sh` — mlcc ≡ mlcc_bootstrap codegen + bootstrap2 self-host **diff_exit=0**.

## STEP=1 — current bootstrap path (2026-05-19)

### Default: `compiler/build.sh`

```
compiler/main.mlc
  → bundle exec ruby ModularCompiler.build   # lib/mlc/
      compile: ModuleGraph → parse (Ruby) → SemanticGen → Cpp HeaderGenerator
      build: wrap_main(main.cpp) → parallel g++ -c → g++ link
  → compiler/out/mlcc
```

| Phase | Implementation today | Ruby-only? |
|-------|---------------------|------------|
| Module graph + dependency order | `ModularCompiler#compile` | yes |
| Parse MLC | `MLC.parse` (`lib/mlc/source/`) | yes |
| Semantic IR | `Semantic::Gen::Pipeline` | yes |
| C++ emit | `Backends::Cpp::HeaderGenerator` | yes |
| `main` → `mlc_user_main` wrap | `wrap_main` in `modular_compiler.rb` | yes |
| Compile `.cpp` + runtime | `g++` via Ruby threads (`MLC_CXX`, `MLC_JOBS`) | no (tooling in Ruby) |
| Link `mlcc` | `g++` | no |

**mlcc-only equivalent (target for STEP=2/5):**

```
compiler/out/mlcc -o compiler/out compiler/main.mlc
compiler/build_bin.sh compiler/out compiler/out/mlcc
```

`build_bin.sh` already matches runtime units (`string.cpp`, `profile.cpp`, `io.cpp`), parallel jobs, `ccache`/`clang++`. Gap vs Ruby path: **`wrap_main`** on entry `main.cpp` (must stay equivalent).

### Optional: `MLCC_BOOTSTRAP=1 compiler/build.sh`

Runs **after** Ruby path produced `compiler/out/mlcc`:

```
compiler/out/mlcc main.mlc -o compiler/out/bootstrap/
  → g++ -c each *.cpp + runtime (sequential, no build_bin.sh)
  → compiler/out/bootstrap/mlcc_bootstrap
```

| Item | Note |
|------|------|
| Entry cwd | `cd compiler && mlcc main.mlc` (relative `main.mlc`) |
| Bootstrap binary path | **`compiler/out/bootstrap/mlcc_bootstrap`** (not `out/mlcc_bootstrap`) |
| Link | inline `g++` loop; no `MLC_CXX`/`ccache`/`build_bin.sh` |
| Failure mode | silent (`2>/dev/null`); no `MLC_REQUIRE_BOOTSTRAP_LINK` |

### Related verification (stability)

| Script | Purpose |
|--------|---------|
| `compiler/build_bin.sh` | Fast parallel link of generated tree → binary |
| `compiler/triple_bootstrap.sh` | mlcc → mlcc2 → mlcc3; `diff bs2 bs3` |
| `compiler/tests/run_mlcc2_checker_parity.sh` | mlcc vs mlcc2 `--check-only` on corpus |
| `compiler/scripts/bisect_bootstrap_link.sh` | git bisect helper for bootstrap link |

### Track deltas (STEP=2–5)

1. **STEP=2** — `build.sh` default: skip Ruby when `compiler/out/mlcc` exists and is fresh (mtime vs `compiler/**/*.mlc`).
2. **STEP=3** — bootstrap block: use `build_bin.sh`; fix gate path; optional CI job.
3. **STEP=4** — assert `mlcc_bootstrap` codegen ≡ `mlcc` (self-host diff).
4. **STEP=5** — Ruby path only via explicit flag or cold-start doc; `rake test_mlc` unchanged.

### Out of scope

- Deleting `lib/mlc/` (reference compiler)
- WASM / cross-compile
