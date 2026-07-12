# Track: Duplicate `extern fn ... from "<header>"` across the import graph → clang error, not diagnostic

Parent: [../FFI_LAYER.md](../../FFI_LAYER.md), [TRACK_EXAMPLES_CI](TRACK_EXAMPLES_CI.md) (closed).
Trigger: 2026-07-11 — `gui_button_demo.mlc` redeclared `extern fn glfw_gl_context_* from "…hpp"` while
transitively importing `gl_window.mlc` with the same binding. Clang failed late; mlcc was silent.

## Status: **closed** (2026-07-12) — Critic OK

**Critic 2026-07-12:** STEP=1–8 vs `d1e6d0fb`…`a5bbe9c2` (+`fae2e6db` link fix); DRY repro+mismatch smokes ok;
`test ! -f docs/agent/TRACK_FFI_EXTERN_DEDUP.md`; PLAN §22 done; FFI_LAYER §10 present.
**reopen: none**. Residual: skip-emit keys on imported **MLC name** (`qualified.has`); E090 keys on `(c_name, header)` — same C symbol under different MLC names still out of Hybrid reuse path.

**Driver 2026-07-12 STEP=8:** `FFI_LAYER.md` §10 + safety table row; TRACK archived; PLAN §22 **done**.
Close → Critic. Delivered: Hybrid skip-emit + E090; REG 20/0; sweep 106/0/1; self-host DIFF=0.

## Decision — **option 3 Hybrid**

Identical `(c_name, header)`+arity → silent reuse (skip second `gen_ffi_fn_decl_cpp` when
`qualified.has(name)`). Conflicting arity → **E090** (`extern_dedup_lint.mlc`).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design decision — pick option 1/2/3 | **done** (Hybrid) |
| 2 | Repro corpus identical redeclare | **done** |
| 3 | Implement Hybrid skip-emit | **done** (`decl_cpp.mlc`) |
| 4 | Negative case → E090 | **done** |
| 5 | Examples sweep | **done** (106/0/1) |
| 6 | Self-host mlcc2 diff | **done** (DIFF=0) |
| 7 | `regression_gate.sh` | **done** (20/0) |
| 8 | Docs `FFI_LAYER.md` + archive | **done** |

## Out of scope

- Auto-`import` rewrite for the user.
- Cross-header aliasing of the same C++ symbol (identity key stays `(c_name, header)`).
