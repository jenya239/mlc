# Track: package manager (design → implementation)

Parent: [../PLAN.md](../PLAN.md) §18. Authorized 2026-07-11 (CONTINUITY backlog).
Queue after [TRACK_TEXT_SHIM_TO_MLC](../archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md) Critic OK.

## Status: **active** (2026-07-13) — STEP=9 done; STEP=10 verify next

Design Steps **1–4** freeze `docs/PACKAGE_MANAGER.md` before any implementation
(Steps 5–10). No `compiler/` until Step 7.

## Next step

**STEP=10** — Verify: self-host if `compiler/` touched; else `regression_gate.sh`.

### STEP=9 done (2026-07-13)

- Usage section in [../PACKAGE_MANAGER.md](../PACKAGE_MANAGER.md) §2.
- README one-liner linking to that doc.

### STEP=8 done (2026-07-13)

- E2E: `compiler/tests/e2e/package_manager/run_e2e.rb` — local bare git →
  `mlc_pkg_fetch` → mlcc compile/link/run → `42`.
- Rebuilt `compiler/out/mlcc` so STEP=7 manifest scan fix is in the binary.

### STEP=7 done (2026-07-13)

- `compiler/driver/path_normalize.mlc`: `pkg/rest` → `.mlc_packages/<pkg>/…`
  when first segment ∈ root `mlc.json` `dependencies`; prefix check after
  `resolve_dotdot`; unknown/escape → relative fallback.
- Tests: `compiler/tests/test_driver.mlc` + `compiler/tests/pkg_resolve_probe.mlc`.
- `.gitignore`: `/.mlc_packages/` (repo root only).

### STEP=6 done (2026-07-13)

- `lib/mlc/package_manager/fetcher.rb` + CLI `scripts/mlc_pkg_fetch.rb`
  (clone/checkout; idempotent `:already_at_rev`; update on rev change).
- `.gitignore`: `.mlc_packages/`
- Tests: `test/mlc/package_manager/fetcher_test.rb` (3 runs, local bare git).

### STEP=5 done (2026-07-13)

- Ruby: `lib/mlc/package_manager/manifest.rb` — `MLC::PackageManager::Manifest`
  load/parse/validate (`[a-z][a-z0-9_]*`, full SHA `rev`, reserved `mlc_packages`,
  missing file → empty deps; unknown keys ignored).
- Tests: `test/mlc/package_manager/manifest_test.rb` (9 runs).

### STEP=4 done (2026-07-13)

- Wrote [../PACKAGE_MANAGER.md](../PACKAGE_MANAGER.md) freezing STEP=1–3 Decisions.
- Design closed for v1; Steps 5–10 implement only what that doc states.

### STEP=3 done (2026-07-13)

- Version skew Decision frozen (below): no compile-time `mlc`/`stdlib` gate;
  optional `mlc_version` deferred.
- No `docs/PACKAGE_MANAGER.md` yet (STEP=4).

### STEP=2 done (2026-07-13)

- Vendor layout + import form + security + flat-deps rule frozen (below).
- No `docs/PACKAGE_MANAGER.md` yet (STEP=4).

### STEP=1 done (2026-07-13)

- Cited current import surface (below).
- Decision frozen (below): `mlc.json` + `{git, rev}` SHA pins; no registry.
- Rejected alternatives table recorded.
- No `docs/PACKAGE_MANAGER.md` yet (STEP=4).

## Decision (STEP=1) — frozen 2026-07-13

### Current import surface (status quo)

| File | Role today |
|------|------------|
| `compiler/driver/path_normalize.mlc` | `resolve_import_path(base, import)` — relative `./` + `.mlc` suffix; bare names only for hard-coded stdlib (`Tcp`/`HttpServer`/`Env`/`Log`/`Validate` → `lib/mlc/common/stdlib/...`); `driver_source_path_is_safe` rejects `..` and unsafe chars |
| `compiler/driver/module_loader.mlc` | Lex/parse graph: each `DeclImport` → `resolve_import_path` → recursive `load_module_impl` on a **filesystem path** under the project/stdlib tree. No package root, no manifest, no vendor dir |

There is **no** third-party package identity in the resolver today.

### Manifest

- **File:** `mlc.json` at the **project root** (same directory the user treats as
  compile root / where `mlcc` is invoked from).
- **Not** `mlc.toml` — Gemfile has no toml gem; Ruby `JSON` is enough for tooling
  (Steps 5–6). Revisit only if a toml parser is vendored later.
- **Minimal schema:**

```json
{
  "name": "my_app",
  "version": "0.1.0",
  "dependencies": {
    "example_pkg": {
      "git": "https://github.com/org/example_pkg.git",
      "rev": "0123456789abcdef0123456789abcdef01234567"
    }
  }
}
```

- `name` / `version`: package identity of **this** project (string; semver
  recommended but not enforced at STEP=1).
- `dependencies`: map keyed by **package name** (filesystem-safe: `[a-z0-9_]+`
  recommended; exact charset frozen at STEP=2 with vendor path rules).
- Each dep: **`git`** (URL string) + **`rev`** (**full 40-char commit SHA**).
  Tags/branches may be used only as a fetch tip in tooling UX; after fetch the
  recorded pin and checkout must be the resolved SHA (detail in STEP=4/6).
- No other keys required. Unknown keys: ignore for forward-compat (STEP=5).
- Empty `dependencies` `{}` is valid.
- Missing `mlc.json`: project has **zero** external packages (current behavior).

### Model rules

1. **No central registry** — deps are only explicit git URLs known to the author.
2. **No semver range solver** — pin is exact SHA; diamond conflicts are the
   author's problem (edit the pin).
3. **Fetch is explicit** (STEP=6 CLI) — compile does not `git clone` on the fly.
4. Vendor layout / import syntax → **STEP=2** (frozen below).

### Rejected alternatives

| Alternative | Why rejected |
|-------------|--------------|
| crates.io / npm-style registry | Hosting, publish, security review — out of proportion |
| Go module proxy + `go.sum` | Needs proxy infra + MVS; pin-SHA is enough now |
| Path-only vendor (`vendor/foo` without manifest) | No reproducible pin; no name→URL record |
| `mlc.toml` | Extra Ruby dependency; JSON stdlib sufficient |
| Live `git` at compile time | Non-hermetic builds; network in `mlcc` hot path |

## Decision (STEP=2) — frozen 2026-07-13

### Vendor layout

- Directory: **`<project_root>/.mlc_packages/<pkg>/`**
- `<pkg>` = key from `mlc.json` `dependencies` (same spelling).
- That directory **is** the git clone root at the pinned `rev` (STEP=6 fetch).
- Package sources live inside that tree (`foo.mlc`, `subdir/bar.mlc`, …).
- `.mlc_packages/` is a **fetched artifact** — default `.gitignore` entry (STEP=4/6);
  not committed to the app repo.
- Reserved: dependency name must not be `mlc_packages` or empty.

### Package name charset

- Exact: **`[a-z][a-z0-9_]*`** (lowercase start; letters, digits, underscore).
- Reject uppercase / `-` / `.` in keys at STEP=5 validate (keeps FS + import parsing simple).

### Import form

Existing syntax stays: `import { … } from '<path>'`.

Resolution order for `<path>` (STEP=7 in `resolve_import_path` / loader):

1. **Relative** — path starts with `.` (`./` or `../`) → current
   `dirname(importer)+path` behavior unchanged.
2. **Bare stdlib** — no `/` → current hard-coded `Tcp`/`HttpServer`/`Env`/`Log`/`Validate`
   table unchanged.
3. **Package** — path contains `/`, does **not** start with `.`, and the
   **first segment** equals a key in root `mlc.json` `dependencies`:
   - `from 'example_pkg/math'` → `.mlc_packages/example_pkg/math.mlc`
   - `from 'example_pkg/util/hash'` → `.mlc_packages/example_pkg/util/hash.mlc`
   - Append `.mlc` if missing (same as today).
4. **Else** — treat as today (join with importer `dirname`). Backward compat for
   accidental `foo/bar` relative imports without `./`.

No new lexer tokens (`pkg:…`, `@pkg/…` rejected for v1 — would need parser work;
slash + first-segment-in-deps is enough).

### Distinguishing project vs dependency

| Kind | How recognized |
|------|----------------|
| Project module | Relative import, or non-dep first segment under importer dir |
| Stdlib | Bare name in the fixed table |
| Dependency module | First path segment ∈ root `dependencies` → under `.mlc_packages/<pkg>/` |

`module_loader` still loads by **filesystem path** after resolve; package identity
exists only in the resolve step + manifest.

### Security

- After resolve, the normalized path for a package import **must** have prefix
  `<project_root>/.mlc_packages/<pkg>/` (no `..` escape out of that package).
- Reuse / extend `driver_source_path_is_safe` + `resolve_dotdot`; add explicit
  prefix check in STEP=7.
- Importing `../` from inside a package cannot leave `.mlc_packages/<pkg>/`.

### Flat dependencies (v1)

- Only the **root project** `mlc.json` `dependencies` are resolvable.
- A package's own `mlc.json` (if present) is **ignored** for resolution in v1.
- Transitive needs: list the transitive package in the root manifest too.
- Nested `dependencies` of packages → later track; not STEP=2–10.

### Rejected import alternatives (STEP=2)

| Alternative | Why rejected |
|-------------|--------------|
| `from 'pkg:module'` / `@pkg/module` | New path grammar / lexer; slash form reuses existing strings |
| Auto-clone on miss at compile | Violates STEP=1 “fetch is explicit” |
| Nested per-package lock / recursive deps | Diamond + complexity; flat root pins only |
| Committing `.mlc_packages/` by default | Noise; SHA pin in `mlc.json` is the source of truth |

## Decision (STEP=3) — frozen 2026-07-13

### Language / stdlib version skew

**Problem:** a dependency may have been authored against a different `mlcc` /
stdlib than the consumer. Mismatch can mean missing APIs, different semantics,
or codegen that no longer type-checks.

**Decision (v1):**

1. **No compile-time version gate.** `mlcc` does **not** read or enforce any
   language/stdlib version field from `mlc.json` or from packages.
2. **Author responsibility.** If a package does not compile with the consumer’s
   toolchain, fix the pin, fork the package, or upgrade `mlcc` — same as today
   for in-tree modules.
3. **Optional future field (deferred):** a root-level `"mlc_version": "<semver>"`
   (or per-dep) may be added later for warnings only. **Not** in the STEP=1
   minimal schema; STEP=5 parser must not require it. Do not implement the
   warning in Steps 5–10.
4. **Stdlib is not a package.** Built-in bare names (`Tcp`, …) always come from
   the consumer’s `lib/mlc/common/stdlib` tree, never from `.mlc_packages/`.
5. **Package↔package ABI:** flat root deps only (STEP=2); no separate ABI
   contract between packages in v1.

### Rejected (STEP=3)

| Alternative | Why rejected |
|-------------|--------------|
| Hard-fail if package `mlc_version` ≠ toolchain | No versioning scheme for `mlcc` releases yet; blocks adoption |
| Embed compiler hash in every package | Unstable; forces rebuild of all pins on every `mlcc` commit |
| Ship stdlib inside each package | Duplicates runtime; conflicts with bare-name stdlib resolve |

## Problem

MLC imports only resolve inside the compile tree (`module_loader` +
`path_normalize`). No way to attach a third-party `.mlc` package by git pin.

## Task

Minimal package model: git URL + pinned rev in a project manifest; explicit
vendor dir; no central registry. Design 1–4, then implement 5–10.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: minimal deps model — `mlc.json` + `{git, rev}` pins, no registry | **done** (2026-07-13) Decision frozen |
| 2 | Design: import path resolution — `.mlc_packages/<name>/` vendor layout vs project root; how `module_loader` distinguishes | **done** (2026-07-13) `pkg/path` + flat deps |
| 3 | Design: language/stdlib version skew — document as known limitation (no ABI gate yet) | **done** (2026-07-13) no version gate; `mlc_version` deferred |
| 4 | Write `docs/PACKAGE_MANAGER.md` freezing STEP=1–3 Decisions | **done** (2026-07-13) design freeze doc |
| 5 | Manifest parser (Ruby): read/validate `mlc.json` schema | **done** (2026-07-13) `Manifest.load` |
| 6 | `scripts/mlc_pkg_fetch.rb` — clone+checkout into `.mlc_packages/`, idempotent | **done** (2026-07-13) Fetcher + CLI |
| 7 | `module_loader` / resolve path — look up deps under `.mlc_packages/` | **done** (2026-07-13) `path_normalize` |
| 8 | E2E smoke: local git fixture package + fetch + compile | **done** (2026-07-13) `run_e2e.rb` → 42 |
| 9 | Docs usage in `PACKAGE_MANAGER.md` + README one-liner | **done** (2026-07-13) usage §2 + README |
| 10 | Verify: self-host if `compiler/` touched; else `regression_gate.sh` | pending |

## Out of scope (hard)

- Central package registry (crates.io-like)
- Semver range / diamond dependency solving
- Package discovery/search (direct git URL only)
- Implementing fetch/loader before STEP=4 design freeze
