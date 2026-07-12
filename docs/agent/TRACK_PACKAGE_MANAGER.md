# Track: package manager (design → implementation)

Parent: [../PLAN.md](../PLAN.md) §18. Authorized 2026-07-11 (CONTINUITY backlog).
Queue after [TRACK_TEXT_SHIM_TO_MLC](../archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md) Critic OK.

## Status: **active** (2026-07-13) — STEP=1 design deps model next

Design Steps **1–4** freeze `docs/PACKAGE_MANAGER.md` before any implementation
(Steps 5–10). No `compiler/`/`lib/mlc/` until Step 5+.

## Next step

**STEP=1** — Design: freeze minimal dependency model (manifest + git+rev, no registry).

### STEP=1 sub-steps (Driver)

1. Cite current import surface: `compiler/driver/path_normalize.mlc` +
   `module_loader.mlc` (project-relative only; no package root today).
2. **Decision — freeze in TRACK** (copy into `PACKAGE_MANAGER.md` at STEP=4):
   - Manifest file name: prefer **`mlc.json`** (stdlib JSON in Ruby tooling; no
     new gem). Reject `mlc.toml` unless a toml parser is already vendored.
   - Schema (minimal): `{ "name": string, "version": string, "dependencies": {
     "<pkg>": { "git": "<url>", "rev": "<commit-sha>" } } }`.
   - Pin = **full commit SHA** (tags allowed as fetch tip but stored/resolved
     as SHA after first fetch — document in STEP=4).
   - No registry, no semver range solver (out of scope already).
3. Record rejected alternatives in one short table (Go modules proxy, crates.io,
   path-only vendor without manifest).
4. Docs-only verify: TRACK Decision block present; PLAN §18 says active STEP=1 done
   after commit — no `docs/PACKAGE_MANAGER.md` yet (that is STEP=4).

## Problem

MLC imports only resolve inside the compile tree (`module_loader` +
`path_normalize`). No way to attach a third-party `.mlc` package by git pin.

## Task

Minimal package model: git URL + pinned rev in a project manifest; explicit
vendor dir; no central registry. Design 1–4, then implement 5–10.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: minimal deps model — `mlc.json` + `{git, rev}` pins, no registry | pending |
| <!-- sub-steps: see STEP=1 above --> | | |
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
