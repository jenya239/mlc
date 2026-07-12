# Track: package manager (design → implementation)

Parent: [../PLAN.md](../PLAN.md) §18. Authorized 2026-07-11 (CONTINUITY backlog).
Queue after [TRACK_TEXT_SHIM_TO_MLC](../archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md) Critic OK.

## Status: **active** (2026-07-13) — STEP=2 done; STEP=3 version-skew design next

Design Steps **1–4** freeze `docs/PACKAGE_MANAGER.md` before any implementation
(Steps 5–10). No `compiler/`/`lib/mlc/` until Step 5+.

## Next step

**STEP=3** — Design: language/stdlib version skew as known limitation (no ABI gate).

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
4. Vendor layout / import syntax → **STEP=2** (not frozen here).

### Rejected alternatives

| Alternative | Why rejected |
|-------------|--------------|
| crates.io / npm-style registry | Hosting, publish, security review — out of proportion |
| Go module proxy + `go.sum` | Needs proxy infra + MVS; pin-SHA is enough now |
| Path-only vendor (`vendor/foo` without manifest) | No reproducible pin; no name→URL record |
| `mlc.toml` | Extra Ruby dependency; JSON stdlib sufficient |
| Live `git` at compile time | Non-hermetic builds; network in `mlcc` hot path |

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
| 2 | Design: import path resolution — `.mlc_packages/<name>/` vendor layout vs project root; how `module_loader` distinguishes | pending |
| <!-- sub-steps: 1) define layout `.mlc_packages/<pkg>/` = clone root; 2) import form `Pkg/foo` or `pkg:foo` — pick one; 3) security: no escape above package root; 4) write Decision into TRACK --> | | |
| 3 | Design: language/stdlib version skew — document as known limitation (no ABI gate yet) | pending |
| <!-- sub-steps: 1) state "no version check at compile"; 2) optional future `mlc_version` field deferred; 3) TRACK Decision only --> | | |
| 4 | Write `docs/PACKAGE_MANAGER.md` freezing STEP=1–3 Decisions | pending |
| 5 | Manifest parser (Ruby): read/validate `mlc.json` schema | pending |
| 6 | `scripts/mlc_pkg_fetch.rb` — clone+checkout into `.mlc_packages/`, idempotent | pending |
| 7 | `module_loader` / resolve path — look up deps under `.mlc_packages/` | pending |
| 8 | E2E smoke: local git fixture package + fetch + compile | pending |
| 9 | Docs usage in `PACKAGE_MANAGER.md` + README one-liner | pending |
| 10 | Verify: self-host if `compiler/` touched; else `regression_gate.sh` | pending |

## Out of scope (hard)

- Central package registry (crates.io-like)
- Semver range / diamond dependency solving
- Package discovery/search (direct git URL only)
- Implementing fetch/loader before STEP=4 design freeze
