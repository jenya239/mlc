# Track: CppStmtFragment elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PREFIX_BRIDGE.md](TRACK_PREFIX_BRIDGE.md) (**closed**, `174b82a`)

## Status: **open**

**Goal:** replace remaining `CppStmtFragment` string snippets in native stmt/expr codegen with structured `CppStmt` nodes (`let_pat_cpp`, `mut_actual_argument`, `return_body` `?` prelude).

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
| 1 | `let_pat_cpp` — PatTuple structured binding without `CppStmtFragment` | done |
| 2 | `let_pat_cpp` — PatRecord / PatArray / PatCtor destructure fragments | done |
| 3 | `mut_actual_argument` — prelude `CppAutoDecl` native (no fragment lines) | done |
| 4 | `return_body` — `?` unwrap prelude as native `CppIf`/`CppReturn` (no fragments) | done |
| 5 | Audit survivors; close track | pending |

## Context

PREFIX_BRIDGE step 5 audit left intentional fragments in `let_pat_cpp.mlc`, `mut_actual_argument.mlc`, `return_body.mlc` (`?`), and `stmt_cpp.mlc` (`gen_stmts_str` print path). This track targets production native paths only; `gen_stmts_str` may remain fragment-backed until a later printer track.

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | template fn + `main()` string decl path |
| `cpp_decl_from_string_output` | extend forwards, non-fn decls |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Native template fn defs — separate track after fragments stable.
- Extend forward segments native CppDecl.
- Phase 4 `MLCC_BOOTSTRAP=1` — after codegen bridges stable.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation — after stability track or parallel Planner pick.
