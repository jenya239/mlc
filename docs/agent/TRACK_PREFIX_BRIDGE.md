# Track: Return-body prefix string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_RETURN_BODY.md](TRACK_RETURN_BODY.md) (**closed**, `91d6752`)

## Status: **open**

**Goal:** remove conservative `return_body_needs_string_bridge` fallback in `gen_fn_decl_cpp` — native `[Shared<CppStmt>]` for fn bodies with prefix stmts (while/for/break) and nested if/match branch blocks. Template/main proto bridge stays (`function_proto_needs_string_bridge`).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Prefix-only return blocks — selective `prefix_statements_need_string_bridge` (while/for/break/return/unit-if prefix); not all prefix stmts | done |
| 2 | While/for/break/continue in return-body prefix — native stmt codegen valid inside `CppFnDef` | done |
| 3 | Nested if/match arms with inner prefix blocks — native branch bodies without string fallback | done |
| 4 | Drop `return_body_needs_string_bridge` from `function_needs_string_bridge`; wire always-native fn defs (non-template) | pending |
| 5 | Audit survivors (`function_proto_needs_string_bridge`, extend forwards, `CppStmtFragment`); close track | pending |

## Context

`gen_return_block_body_cpp` already emits prefix stmts via `gen_stmts_cpp_with_context`, but `expression_needs_string_bridge` still returns true when `block_statements.length() > 0`, forcing `gen_fn_decl` string path. Step 4 RETURN_BODY added this after native path broke on while/if-in-branch (e.g. `parse_array_pattern` — `break` not in loop).

## Survivors (intentional, out of scope unless step 5 notes)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | template fns + `main()` proto |
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
