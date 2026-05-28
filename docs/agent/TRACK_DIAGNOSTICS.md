# Track: Diagnostics — error codes (Phase 1 continuation)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §1; previous: [TRACK_SECURITY.md](TRACK_SECURITY.md) (**closed**, `a035c3d`)

## Status: **closed** (step 5 `f6863fe`)

**Goal:** assign stable `error[Exxx]:` codes to high-traffic checker paths; negative tests assert exact format.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.

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
| 1 | Code catalog — `diagnostic_codes.mlc` (E001–E020); wire imports; smoke test | done (`b44411c`) |
| 2 | Binary / type mismatch — `binary_diagnostics.mlc`, `type_diagnostics.mlc` | done (`db80520`) |
| 3 | Method receiver — `method_receiver_diagnostics.mlc` → E021–E029 | done (`2ae66ae`) |
| 4 | Infer/transform hot paths — E030–E033 | done (`fc78103`) |
| 5 | Audit sweep — `infer_call.mlc` named args → E034–E035 | done (`f6863fe`) |

## Step 5 detail (done — `f6863fe`)

- `infer_call.mlc`: E034 unknown parameter name, E035 duplicate named arg.
- Catalog extended to 35 codes; exact negative tests E034–E035.
- Gate: 692 pass; self-host diff empty.

## Deferred (not in this track)

- Remaining bare `diagnostic_error(` in checker (`check.mlc`, `infer.mlc`, …) — future track.
- `infer_question_expression.mlc` ? operator — no code yet.
- Parser panic audit; differential fuzz; `lib/mlc/` parity.

## Track closed

All 5 steps done. Next: **Planner** `STEP=plan-refresh`.
