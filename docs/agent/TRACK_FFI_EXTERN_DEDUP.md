# Track: Duplicate `extern fn ... from "<header>"` across the import graph → clang error, not diagnostic

Parent: [../FFI_LAYER.md](../FFI_LAYER.md), [TRACK_EXAMPLES_CI](../archive/tracks/TRACK_EXAMPLES_CI.md) (closed).
Trigger: 2026-07-11 — `gui_button_demo.mlc` redeclared `extern fn glfw_gl_context_* from "…hpp"` while
transitively importing `gl_window.mlc` with the same binding. Clang failed late; mlcc was silent.

## Status: **active** — STEP=1–5 **done**; STEP=6 next

## Next step

**STEP=6** — self-host verify (mlcc → mlcc2 diff identical).

## Problem

Codegen emits `extern fn ... from "<header>"` inside each declaring MLC module's C++ namespace.
Two modules declaring the same `(c_name, header)` → clang: redeclaration across namespaces.
No mlcc diagnostic; failure only at C++ compile.

## Decision (STEP=1, 2026-07-12) — **option 3 Hybrid**

Facts from read:
- Emit: `gen_ffi_fn_decl_cpp` in `compiler/codegen/decl_cpp.mlc:694-729` — per-module wrapper + `static_cast<…>(&::<c_name>)`; includes via `collect_ffi_include_lines` / `ffi_header_include_line` (~603–618).
- Namespace wrap: `module.mlc:184-213` — each TU opens `namespace <module_base>`; FFI defs land inside that namespace (`assemble_*` + `collect_fn_defs_cpp`).
- Existing checker: `extern_header_arity_lint.mlc` only vs header-import stubs (`W-EXTERN-ARITY`); no cross-module `(c_name, header)` index.

Why not 1: STEP=2 acceptance is identical redeclare **passes** after fix — pure checker-error on every duplicate would fail that gate and break copy-paste leaf demos that also transitively import `gl_window.mlc`.
Why not 2 alone: shared/global C++ namespace for all FFI binders is higher blast radius (placement of includes, call resolution, self-host surface) without buying mismatch diagnostics.
**Hybrid:** index `(c_name, header)` across the load graph; matching signatures → silent reuse (skip second `gen_ffi_fn_decl_cpp` / bind to first export); mismatch → mlcc error with both sites (STEP=4). Prefer reuse-via-import when an imported export already owns the key.

## Root cause candidates (STEP=1 picked 3)

1. Checker: detect duplicate `(c_name, header)` → merge or error pointing both sites.
2. Codegen: emit once into shared/global namespace keyed by `(c_name, header)` (prefer if low-risk).
3. Hybrid: silent reuse when signatures match; mlcc error when same key, different signature. **← chosen**

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design decision — read codegen/checker for `extern fn ... from`; pick option 1/2/3; write 2–3 sentences here | **done** (option 3 Hybrid) |
| 2 | Repro corpus: two-module e2e fixture (identical signature redeclare) — fails clang today, passes after fix | **done** (`fixtures/extern_dedup/` + `run_extern_dedup_repro.sh` expects enclose-namespace fail) |
| 3 | Implement chosen dedup/diagnostic in checker and/or codegen | **done** (`ffi_extern_reuses_imported_binding` skip emit in `decl_cpp.mlc`) |
| 4 | Negative case: same key, different signatures → mlcc error with both sites | **done** (E090; later site + earlier label in message) |
| 5 | Re-check examples sweep / button demos still OK | **pending** |
| 6 | Self-host verify (mlcc → mlcc2 diff identical) | pending |
| 7 | `scripts/regression_gate.sh` green | pending |
| 8 | Docs: `FFI_LAYER.md` one paragraph on dedup rule | pending |

### STEP=3 delivery

- `compiler/codegen/decl_cpp.mlc`: `ffi_extern_reuses_imported_binding` — if `context.qualified.has(name)`, skip FFI proto/def; local emit uses `cpp_safe(name)` not `resolve`.
- `run_extern_dedup_repro.sh` expects build+run exit 0.

### STEP=4 delivery

- `extern_dedup_lint.mlc` + E090; wired in `check.mlc`.
- Fixture `fixtures/extern_dedup/mismatch.mlc`; `run_extern_dedup_mismatch_smoke.sh`.
- Identical redeclare stays clean; message names earlier site (`provider.mlc:…`).
- Also: `extern_header_arity_lint` index via `let mut result` (avoid match→HashMap codegen void bug).

### STEP=5 sub-steps (Driver)

1. `scripts/run_examples_compile_sweep.sh` (or gated subset with gui_button_demo).
2. Confirm no new fails vs STEP=3 baseline.
3. `next` = STEP=6.

## Out of scope

- Auto-`import` rewrite for the user.
- Cross-header aliasing of the same C++ symbol (identity key stays `(c_name, header)`).
