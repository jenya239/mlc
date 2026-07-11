# Track: Duplicate `extern fn ... from "<header>"` across the import graph → clang error, not diagnostic

Parent: [../FFI_LAYER.md](../FFI_LAYER.md), [TRACK_EXAMPLES_CI](TRACK_EXAMPLES_CI.md).
Trigger: 2026-07-11 (несвязанная задача — демо). `misc/examples/gui_button_demo.mlc`
declared `extern fn glfw_gl_context_begin(...) = "mlc::gl::glfw_gl_context_begin"
from "mlc/gl/glfw_window_gl.hpp"` directly in the main module, while
transitively importing `misc/gui/input.mlc` → `lib/mlc/common/stdlib/gl/gl_window.mlc`,
which **also** declares the same extern (same C++ symbol, same header) under
`export extern fn`. Result: clang error at C++ compile time —
`cannot define or redeclare 'glfw_gl_context_begin' here because namespace
'gui_button_demo' does not enclose namespace 'gl_window'` — not an mlcc-level
diagnostic. The `.mlc` file compiled to C++ with **zero mlcc warnings/errors**;
the failure only surfaced at clang link stage, deep in generated `.cpp`.

## Status: open, средний приоритет

## Problem

Codegen currently emits a C++ forward-declaration + extern "C"-style binding
for every `extern fn ... from "<header>"` **inside the namespace of the
declaring MLC module**. When two different MLC modules in the same program
both declare an extern binding for the same underlying C++ symbol (same
mangled name / same header), each module's codegen tries to declare it in
its own namespace — clang rejects the second one, because the symbol was
already introduced (via `#include` + first declaration) in an incompatible
namespace scope. This is a structural gap, not specific to GLFW: any two
modules independently declaring `extern fn foo() = "bar::foo" from
"bar.hpp"` will hit it, silently, only at the C++ compile stage.

## Root cause candidates (STEP=1 must pick one, not assume)

1. Checker should detect duplicate `(c_name, header)` pairs across the whole
   compiled module graph at check time and either (a) merge them into a
   single declaration site, or (b) emit a checker error pointing at both
   declaration sites, telling the user to `import` the existing binding
   instead of re-declaring it.
2. Codegen could emit the extern declaration into a shared/global namespace
   (not per-module) keyed by `(c_name, header)`, deduplicated — no error,
   works automatically regardless of which modules re-declare it. Prefer
   this over (1)'s error if it is low-risk: it matches how C++ headers work
   naturally (one declaration, `#include` guards prevent duplication) and
   removes an entire class of footguns without requiring user action.
3. Hybrid: dedupe when the signature matches exactly (same param/return
   types) — silently reuse the first declaration; error only when two
   modules declare the **same** `(c_name, header)` with **different**
   signatures (real conflict, not redundant declaration).

Read `compiler/codegen/decl.mlc` (or wherever `extern fn ... from` codegen
lives — grep `from_header`/`c_name` in `compiler/checker/` and
`compiler/codegen/`) before picking; do not guess file names.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design decision — read current codegen for `extern fn ... from "<header>"`, confirm which of the 3 root-cause options is lowest-risk given how namespaces are currently emitted per-module; write the decision (2-3 sentences) into this track before touching code | pending |
| 2 | Repro corpus: two-module minimal case (module A declares `extern fn foo() from "x.hpp"`, module B imports A and separately re-declares the same `extern fn foo() from "x.hpp"` with identical signature) as an e2e fixture under `compiler/tests/e2e/` — must fail today (clang error), must pass after fix | pending |
| 3 | Implement chosen dedup/diagnostic strategy in checker and/or codegen | pending |
| 4 | Negative case: same `(c_name, header)` declared with **different** signatures across two modules → clear mlcc-level error (not silent, not a clang error) with both declaration sites in the message | pending |
| 5 | Re-run `TRACK_EXAMPLES_CI` sweep (or manual compile of `gui_button_demo.mlc`/`gui_button_interactive_demo.mlc`/`text_dashboard_demo.mlc`) to confirm the workaround (explicit `import` instead of redeclaring) still works and the new fixture from Step 2 also works without the workaround | pending |
| 6 | Self-host verify: `compiler/out/mlcc` → `mlcc2`, diff identical (touches checker/codegen) | pending |
| 7 | `scripts/regression_gate.sh` green | pending |
| 8 | Docs: `FFI_LAYER.md` — one paragraph documenting the dedup rule (same signature = reuse silently, different signature = error) | pending |

## Out of scope

- Auto-generating `import` statements for the user (still must write
  `import { foo } from '...'` themselves, or re-declare with a matching
  signature — this track only makes re-declaration with a **matching**
  signature safe instead of a late clang error).
- Cross-header symbol aliasing (same C++ symbol reachable via two different
  header paths) — out of scope, treat `(c_name, header)` as the identity key.
