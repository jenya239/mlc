# Track: Diagnostics — error codes (Phase 1 continuation)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_SECURITY.md](TRACK_SECURITY.md) (**closed**, `a035c3d`)

## Status: **active** (step 5 pending)

**Goal:** assign stable `error[Exxx]:` codes to high-traffic checker paths; negative tests assert exact format. E001 (undefined), E002 (not callable), E003 (arg mismatch) already exist — bulk migration deferred from [TRACK_PHASE1.md](TRACK_PHASE1.md) step 2.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Do not rename messages without updating matching negative tests.

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 680 pass (baseline post-SECURITY)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Code catalog — `diagnostic_codes.mlc` (E001–E020 constants + doc); wire imports; smoke test in `test_checker.mlc` | done (`b44411c`) |
| 2 | Binary / type mismatch — `binary_diagnostics.mlc`, `type_diagnostics.mlc` → `diagnostic_error_with_code`; negative tests | done (`db80520`) |
| 3 | Method receiver — `method_receiver_diagnostics.mlc` → E021–E029; negative tests | done (`2ae66ae`) |
| 4 | Infer/transform hot paths — record missing field, immut assign, match guard, trait bounds → codes + tests | pending |
| 5 | Audit sweep — grep remaining bare `diagnostic_error(` in checker; fix one module batch + tests | pending |

## Step 1 detail (done)

- `compiler/checker/diagnostic_codes.mlc`: E001–E020 + `diagnostic_code_catalog_count`.
- Wired E001–E004 at names, infer_call_support, call_argument_unify, check.mlc.
- Gate: 683 pass; self-host diff empty (`b44411c`).

## Step 1 detail (spec)

- File: `compiler/checker/diagnostic_codes.mlc` (or `compiler/frontend/diagnostic_codes.mlc` if zero checker deps).
- Export string constants `E001`…`E020` with comment mapping (message category).
- Replace literal `'E001'` / `'E002'` / `'E003'` at existing call sites with constants (same behavior).
- `test_checker.mlc`: assert catalog exports non-empty; existing E001/E002/E003 tests unchanged.

## Deferred (not in this track)

- Parser panic audit — lexer done in SECURITY; parser OOB fixed in SAFETY step 4.
- Differential fuzz mlcc vs Ruby — [RESEARCH.md](RESEARCH.md).
- `lib/mlc/` Ruby parity for codes — not in scope.
- Phase 3 fmt/LSP — new TRACK after diagnostics stable.

## Step 2 detail (done — `db80520`)

- `binary_diagnostics.mlc`: E005–E011 for binary operand errors.
- `type_diagnostics.mlc`: E012–E020 for unary, arity, field, index, if/for/match.
- `test_checker.mlc`: exact negative tests E005, E014–E016; fixed undefined-field assert.
- Gate: 686 pass; self-host diff empty.

## Step 3 detail (done — `2ae66ae`)

- `method_receiver_diagnostics.mlc`: E021–E029 for builtin method receiver errors.
- `diagnostic_codes.mlc`: catalog extended to 29 codes.
- `test_checker.mlc`: exact E027/E021; has-on-sum-type uses E027.
- Gate: 687 pass; self-host diff empty.

## Next step (Driver)

**STEP=4** — infer/transform hot paths → codes.
