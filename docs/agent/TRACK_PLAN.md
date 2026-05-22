# Track: PLAN.md roadmap (post-optimization)

Parent: [../PLAN.md](../PLAN.md)

## Status: not started

Optimization batch staged for commit ([TRACK_OPTIMIZATION.md](TRACK_OPTIMIZATION.md)).

## Next verifiable steps (PLAN §3, порядок внедрения)

1. [ ] `trait Pass<Input, Output>` — новый модуль `compiler/pass.mlc`, только тип + комментарий, без wiring.
2. [ ] `ExprVisitor` sketch — отдельный файл, dispatch stub.
3. [ ] Parser `ref mut` — **отдельная ветка** (bootstrap limit).

## Current step

Ждём commit optimization → `record_baseline.sh` → step 1.
