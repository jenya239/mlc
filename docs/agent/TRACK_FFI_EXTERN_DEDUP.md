# Track: Duplicate `extern fn ... from "<header>"` across the import graph → clang error, not diagnostic

Parent: [../FFI_LAYER.md](../FFI_LAYER.md), [TRACK_EXAMPLES_CI](../archive/tracks/TRACK_EXAMPLES_CI.md) (closed).
Trigger: 2026-07-11 — `gui_button_demo.mlc` redeclared `extern fn glfw_gl_context_* from "…hpp"` while
transitively importing `gl_window.mlc` with the same binding. Clang failed late; mlcc was silent.

## Status: **active** (Planner 2026-07-12) — очередь §22

## Next step

**STEP=1** — design decision among root-cause options 1/2/3 (docs only; no `compiler/` yet).

## Problem

Codegen emits `extern fn ... from "<header>"` inside each declaring MLC module's C++ namespace.
Two modules declaring the same `(c_name, header)` → clang: redeclaration across namespaces.
No mlcc diagnostic; failure only at C++ compile.

## Root cause candidates (STEP=1 must pick one)

1. Checker: detect duplicate `(c_name, header)` → merge or error pointing both sites.
2. Codegen: emit once into shared/global namespace keyed by `(c_name, header)` (prefer if low-risk).
3. Hybrid: silent reuse when signatures match; mlcc error when same key, different signature.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design decision — read codegen/checker for `extern fn ... from`; pick option 1/2/3; write 2–3 sentences here | **pending** |
| 2 | Repro corpus: two-module e2e fixture (identical signature redeclare) — fails clang today, passes after fix | pending |
| 3 | Implement chosen dedup/diagnostic in checker and/or codegen | pending |
| 4 | Negative case: same key, different signatures → mlcc error with both sites | pending |
| 5 | Re-check examples sweep / button demos still OK | pending |
| 6 | Self-host verify (mlcc → mlcc2 diff identical) | pending |
| 7 | `scripts/regression_gate.sh` green | pending |
| 8 | Docs: `FFI_LAYER.md` one paragraph on dedup rule | pending |

### STEP=1 sub-steps (Driver)

1. Grep `from_header` / `extern` emit in `compiler/codegen/` + `compiler/checker/` (and Ruby parity if needed for notes only).
2. Note how per-module C++ namespaces wrap extern decls today (file:line).
3. Pick option **1 / 2 / 3** with risk rationale; update this TRACK Status/Decision section.
4. No code change this step — `next` = STEP=2 after commit.

## Out of scope

- Auto-`import` rewrite for the user.
- Cross-header aliasing of the same C++ symbol (identity key stays `(c_name, header)`).
