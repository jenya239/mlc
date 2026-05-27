# Track: CppAST codegen cleanup (Phase 2 follow-up)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_CPPEXPR.md](TRACK_CPPEXPR.md) (**closed**, step 21 `96df4e0`)

## Status: in progress (step 6 pending)

**Goal:** eliminate remaining string concatenation in codegen hot path. Default emission is CppExpr/printer (step 20); survivors: `expr_fragment_codegen` string-bridge in `expr_visitor_cpp`, `codegen/expr/expr.mlc` templates in stmt/decl/match/module, dual `gen_module_via_string`.

**Constraints:**
- One layer per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Unique module basenames (`path_to_module_base`).
- Self-host link: **g++** + `runtime/src/core/profile.cpp`.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 518 pass (baseline post-CPPEXPR)
compiler/build.sh
compiler/out/mlcc -o $TMP/mlc_p1 compiler/main.mlc
g++ -std=c++20 -O2 -I $TMP/mlc_p1 -I runtime/include $TMP/mlc_p1/*.cpp \
  runtime/src/io/io.cpp runtime/src/core/string.cpp runtime/src/core/profile.cpp \
  -o $TMP/mlcc2
$TMP/mlcc2 -o $TMP/mlc_p2 compiler/main.mlc
diff -rq $TMP/mlc_p1 $TMP/mlc_p2   # must stay empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `expr_visitor_cpp` — field + index as `CppMember`/`CppIndex` (drop fragment bridge) | done (`4b81869`) |
| 2 | `expr_visitor_cpp` — while/for as CppExpr (no `expr_fragment_codegen`) | done (`a2830b9`) |
| 3 | `expr_visitor_cpp` — tuple/question/with + bin/unary fallback native CppExpr | done (`fc919bc`) |
| 4 | `match_gen` / record — CppStdVisit + record init via CppExpr in visitor (not string templates) | done (`d9ba2fe`) |
| 5 | `stmt_cpp` — internal CppStmt only; string `gen_stmts_str` thin print wrapper at eval boundary | done (`7164b5a`) |
| 6 | `decl_cpp` — migrate remaining `decl.mlc` string emission | pending |
| 7 | `module.mlc` — remove `gen_module_via_string`; drop `use_cpp_printer` flag | pending |
| 8 | `expr.mlc` — delete unused templates; shrink or inline survivors | pending |
| 9 | `cpp_printer.mlc` — formatting polish (indent, breaks) beyond deterministic baseline | pending |
| 10 | Remove dead dual-backend helpers (`eval_expr_cpp_as_string` exports if unused) | pending |

## Step details

### Step 1 — field + index

- File: `compiler/expr_visitor_cpp.mlc` (+ tests in `test_codegen.mlc` if needed).
- Replace `cpp_expr_from_rendered_fragment(gen_*_via_visitor)` for field/index with direct AST nodes.
- No changes to `expr.mlc` yet.

### Steps 2–3 — remaining expr bridges

- One batch per step; gate after each.
- `expr_fragment_codegen.mlc` shrinks; delete when empty.

### Steps 4–7 — stmt / decl / module

- One subdirectory concern per step.
- Step 7: `use_cpp_printer_default()` always true → delete string module path.

### Steps 8–10 — cleanup

- Step 8: grep-driven deletion; keep templates still needed for bootstrap edge cases until step 10.
- Step 9: golden tests in `test_cpp_printer.mlc`.
- Step 10: public API surface = CppFile / print only.

## Deferred

- Parser `ref mut` — separate branch (TRACK_PLAN step 15).
- `lib/mlc/` Ruby backend parity — not in this track.
- Phase 1 (diagnostics span, fuzzing, out-dir) — separate TRACK when CppGEN closed.

## Next step (Driver)

**STEP=6** — decl_cpp migrate remaining decl.mlc string emission.
