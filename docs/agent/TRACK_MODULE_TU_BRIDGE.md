# Track: Native module TU scaffolding (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DERIVE_HASH_STMT_BRIDGE.md](TRACK_DERIVE_HASH_STMT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 6 commit — 2026-05-19 Driver)

**STEP=6 note (2026-05-19 Driver):** Committed `module.mlc`, `test_cpp_printer.mlc`, `module_tu_helpers.mlc`, `cpp_ast.mlc`, `cpp_printer.mlc`, `cpp_naming.mlc`. Gate: build_tests **940**/0; build.sh OK; self-host diff N/A (`build_bin` pre-existing).

**Critic audit (2026-06-04):** Steps 4–5 verified on worktree; step 6 landed in git.

**STEP=5 audit (2026-06-04 Driver):**

| Check | Result |
|-------|--------|
| `cpp_decl_from_string_output` in `module.mlc` | **0** (removed with `cpp_declarations_from_text_parts`) |
| `cpp_declarations_from_text_parts` | **0** in repo |
| TU assembly fragments (intentional) | `module.mlc`: `using_namespace_lines`, `cli_wrapper` → `make_fragment_cpp_declaration`; `module_tu_helpers`: blank line → fragment |
| `expr.mlc:284-306` TU line helpers | **0** production callers (dead; `std_includes` / `bootstrap_host_main` still string-built in `prepare_module_generation`) |
| `decl_cpp.cpp_decl_from_string_output` | export only; **0** production callers |
| Gate | build_tests **937**/0; build.sh OK; self-host diff N/A (`build_bin` pre-existing `exprs.cpp` destructuring) |

**STEP=4 note (2026-06-04 Driver):** `assemble_source_cpp_declarations` native prologue/epilogue (`CppDefineMacro`, `CppInclude`, `CppNamespaceBegin`/`CppNamespaceEnd`); `implementation_import_includes` via `append_cpp_declarations_from_include_text`; survivors: `using_namespace_lines` + `cli_wrapper` → single `CppDeclFragment` each. Removed `cpp_declarations_from_text_parts` from `module.mlc`. Golden: `test_cpp_printer.mlc` source imports/using. Gate: build_tests **937**/0, build.sh OK; self-host diff pre-existing FAIL.

**STEP=3 note (2026-06-03 Driver):** `assemble_header_cpp_declarations` already native; zero text_parts in header path.

**Goal:** eliminate `cpp_decl_from_string_output` in `module.mlc` TU assembly — native `CppDecl` for include guards, `#include`, namespace wrapper, and entry `main` shim instead of `CppDeclFragment` text parts.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Reachability audit — `cpp_declarations_from_text_parts`; text helpers; tests | done |
| 2 | Native decl helpers (include guard, `#include`, namespace open/close, `#define main`) | done |
| 3 | `assemble_header_cpp_declarations` → native nodes | done |
| 4 | `assemble_source_cpp_declarations` + cli wrapper; parity tests | done |
| 5 | Survivors audit; close track | done |
| 6 | Commit steps 4–5 WIP (`module.mlc`, `test_cpp_printer.mlc`) | done |

<!-- step 1 sub-steps:
  1) grep `cpp_decl_from_string_output` / `cpp_declarations_from_text_parts` in module.mlc
  2) trace `assemble_header_cpp_declarations` / `assemble_source_cpp_declarations` text_parts
  3) note expr.mlc helpers :284-306; existing module tests if any
-->

<!-- step 2 sub-steps:
  1) CppInclude already in cpp_ast — multi-line include string → [CppInclude]
  2) include guard + #define — CppDeclFragment vs new preprocessor nodes (minimal)
  3) CppNamespace wrapper for decl_parts body
-->

## Context (Planner 2026-06-03)

| Site | Pattern |
|------|---------|
| `module.mlc:181-191` | `cpp_declarations_from_text_parts` → `cpp_decl_from_string_output` |
| `module.mlc:194-210` | header: guard, std includes, namespace open/close |
| `module.mlc:214-230` | source: `#define main`, quote-include, imports, namespace, cli_wrapper |
| `expr.mlc:284-306` | string helpers for guard/namespace/include lines |
| `decl_cpp.mlc:27-28` | `cpp_decl_from_string_output` → `make_fragment_cpp_declaration` |

Prior work: derive + decl bridge tracks closed; Hash native path complete. **module TU** is the main remaining production consumer of `cpp_decl_from_string_output`.

**Why this track:** bounded file (`module.mlc`); every modular `.hpp`/`.cpp` passes through TU scaffolding; stability before Phase 4 bootstrap.

## Deferred (out of track)

- `type_gen.mlc` string derive_hash parallel path.
- `expr_visitor_cpp.mlc` unit-if `SExprBlock` fragment (`:196`).
- `let_pat_cpp.mlc` structured-binding fallback fragments.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

## Step 1 audit (2026-06-03)

| Check | Result |
|-------|--------|
| `cpp_decl_from_string_output` call sites in `compiler/` | **1** consumer — `module.mlc:186` via `cpp_declarations_from_text_parts` |
| `cpp_declarations_from_text_parts` call sites | **4** — header prologue `:194`, header epilogue `:205`, source prologue `:214`, source epilogue `:226` |
| Non-empty text parts → fragments (header) | **5** — `#ifndef`, `#define` guard, `std_includes`, `namespace {`, close + `#endif` |
| Non-empty text parts → fragments (source) | **5–7** — `#define main`, quote-`#include`, import includes, `namespace {`, using lines (optional), close, `cli_wrapper` (entry only) |
| Native decl body (between scaffolding) | `decl_parts.type_fwds/type_defs/fn_protos` (header), `decl_parts.fn_defs` (source) — already native |
| String helpers | `expr.mlc:281-306` (guard/namespace/main shim); `cpp_naming.mlc:94-119` (`include_lines`, `using_namespace_lines`) |
| Production path | `pipeline.mlc:84` → `gen_module` → `gen_module_cpp_file` → `assemble_*` → `print_cpp_declarations` |
| Test harness | `gen_program` in `module.mlc:252-259` (single TU) |
| TU scaffolding tests | **0** anti-fragment — `test_codegen.mlc` uses `gen_program` string contains only; no `CppDeclFragment` assertion on TU parts |
| `cpp_decls_from_string_parts` | `decl_cpp.mlc:34` — export; **0** production callers besides module path |

Result: **928 pass; DIFF_EXIT=0**.

**Step 2:** `module_tu_helpers.mlc` + `CppIfndef`/`CppDefineMacro`/`CppEndif`/`CppNamespaceBegin`/`CppNamespaceEnd` in cpp_ast/cpp_printer; golden tests in `test_cpp_printer.mlc`. Result: **936 pass; DIFF_EXIT=0**.
