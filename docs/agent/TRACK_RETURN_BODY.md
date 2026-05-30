# Track: Return-body string bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_STMT_BRIDGE.md](TRACK_STMT_BRIDGE.md) (**closed**, `7084227`)

## Status: **closed**

**Goal:** eliminate `cpp_stmts_from_string_output(gen_return_body(...))` in `codegen/stmt/return_body.mlc` — native `[Shared<CppStmt>]` for function/return bodies used by `decl_cpp.mlc`.

---

| Step | Item | Status |
|------|------|--------|
| 1 | Leaf returns — unit, direct expr, `?` as native CppReturn/CppBlock | done (`1fce706`) |
| 2 | `SExprBlock` return body — stmts via `gen_stmts_cpp` + trailing return | done (`9f34e4d`) |
| 3 | Return if/else-if chains — native CppIf (no string fragment) | done (`719edbb`) |
| 4 | `gen_return_body_cpp` fully native; `decl_cpp` wired; drop string round-trip | done (`993a467`) |
| 5 | Audit `cpp_stmts_from_string_output` callers; close track | done |

## Step 5 audit (`cpp_stmts_from_string_output`)

**Track goal met:** no `cpp_stmts_from_string_output(gen_return_body(...))` in `return_body.mlc` (removed step 4).

| Symbol | Callers in `compiler/**/*.mlc` | Notes |
|--------|-------------------------------|-------|
| `cpp_stmts_from_string_output` | **0** (definition only in `stmt_cpp.mlc`) | kept for API parity / future stmt bundles |
| `cpp_stmt_from_string_output` | internal to `stmt_cpp.mlc` | fragment wrapper |
| `gen_return_body` (string) | `return_body.mlc` string path + `decl.mlc` via `gen_fn_body` | parallel path; bridge fallback in `gen_fn_decl_cpp` |

Production fn bodies: `module.mlc` → `collect_fn_defs_cpp` → `gen_fn_decl_cpp` → `gen_return_body_cpp` (native) or string bridge when `return_body_needs_string_bridge`.

## Survivors (intentional)

| Site | Use |
|------|-----|
| `return_body_needs_string_bridge` | complex prefix stmts / nested if/while → `gen_fn_decl` string in `gen_fn_decl_cpp` |
| `decl.mlc` | string `gen_fn_body` / `gen_decl` parallel path |
| `collect_fn_defs_cpp` | extend forward segments via `cpp_decl_from_string_output` |
| `let_pat_cpp.mlc` | structured binding fragments (TRACK_STMT_BRIDGE survivor) |
| `mut_actual_argument.mlc` | mut-ref prelude fragments |

## Deferred (out of track)

- Native prefix stmts for while/for/if-in-branch (remove `return_body_needs_string_bridge`).
- `let_pat_cpp.mlc` internal `CppStmtFragment`.
- `mut_actual_argument.mlc` mut-ref prelude fragments.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
