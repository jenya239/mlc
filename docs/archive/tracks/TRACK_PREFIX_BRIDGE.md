# Track: Return-body prefix string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_RETURN_BODY.md](TRACK_RETURN_BODY.md) (**closed**, `91d6752`)

## Status: **closed** (`519d19b` step 4 + step 5 audit)

**Goal:** remove conservative `return_body_needs_string_bridge` fallback in `gen_fn_decl_cpp` — native `[Shared<CppStmt>]` for fn bodies with prefix stmts (while/for/break) and nested if/match branch blocks.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Prefix-only return blocks — selective `prefix_statements_need_string_bridge` (while/for/break/return/unit-if prefix); not all prefix stmts | done (`b1d88c7`) |
| 2 | While/for/break/continue in return-body prefix — native stmt codegen valid inside `CppFnDef` | done (`7969531`) |
| 3 | Nested if/match arms with inner prefix blocks — native branch bodies without string fallback | done (`e8edb18`) |
| 4 | Drop `return_body_needs_string_bridge` from `function_needs_string_bridge`; wire always-native fn defs (non-template) | done (`519d19b`) |
| 5 | Audit survivors (`function_proto_needs_string_bridge`, extend forwards, `CppStmtFragment`); close track | done |

## Step 5 audit

**Track goal met:** non-template fn defs always use `native_fn_decl_cpp` → `gen_return_body_cpp`; `return_body_needs_string_bridge` removed.

| Symbol | Callers in `compiler/**/*.mlc` | Notes |
|--------|-------------------------------|-------|
| `function_proto_needs_string_bridge` | `decl_cpp.mlc` (`gen_fn_proto_cpp`, `gen_fn_decl_cpp`) | template fns + `main()` proto/body still string path |
| `cpp_decl_from_string_output` | `decl_cpp.mlc`, `module.mlc`, `collect_fn_defs_cpp` | extend forward segments; non-fn decls; template fn fallback |
| `CppStmtFragment` | `let_pat_cpp.mlc`, `mut_actual_argument.mlc`, `return_body.mlc` (`?` unwrap), `stmt_cpp.mlc` | structured binding / mut-ref / try prelude; `gen_stmts_str` print path |
| `gen_return_body` (string) | `return_body.mlc`, `decl.mlc` via `gen_fn_body` | parallel path for template fn bodies and non-fn decl string codegen |

Production fn bodies (non-template): `collect_fn_defs_cpp` → `gen_fn_decl_cpp` → `native_fn_decl_cpp` → `gen_return_body_cpp`.

## Survivors (intentional)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | template fns + `main()` proto/body |
| `collect_fn_defs_cpp` | extend forward segments via `cpp_decl_from_string_output` |
| `let_pat_cpp.mlc` | structured binding `CppStmtFragment` |
| `mut_actual_argument.mlc` | mut-ref prelude fragments |
| `decl.mlc` | string parallel path for non-fn decls |

## Deferred (out of track)

- Native template fn defs (`function_proto_needs_string_bridge` body side).
- Extend forward segments native CppDecl.
- `let_pat_cpp` / `mut_actual_argument` fragment elimination — separate tracks.
- Phase 4 `MLCC_BOOTSTRAP=1` — after stmt bridges stable.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
