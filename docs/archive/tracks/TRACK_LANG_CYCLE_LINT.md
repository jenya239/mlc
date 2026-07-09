# Track: cycle lint — warning on `Shared` type cycles without `Weak`

Parent: [../PLAN.md](../PLAN.md). Depends on:
[TRACK_LANG_WEAK_SUGAR.md](TRACK_LANG_WEAK_SUGAR.md) (эргономика `Weak` должна
существовать раньше, чем линтер начнёт советовать её использовать). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #5 (Уровень 1 из
части «память и связность»).

## Status: **closed** (2026-07-09)

**Не полный alias-анализ.** Узкая диагностика уровня lint (см. `docs/MLC.md`
план линтера — `G1 → G2 (минимальные правила) → ...`), не блокирующая ошибка.

### Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | `cycle_lint.mlc` + wire in `check.mlc`; `W-CYCLE`; errors-only fail `check`; tests in `test_checker.mlc` |
| 2 | **done** | `CheckOut.warnings`; `eprintln` in `run_checker_pass`; builtin `eprintln` + `map_builtin`; `--check-only` prints `warning[W-CYCLE]` |
| 3 | **done** | Self-apply: `mlcc --check-only compiler/main.mlc` → **0** `W-CYCLE` / warnings (exit 0). No false positives. |
| 4 | **done** | verify-gate: self-host p1/mlcc2/p2 identical; `regression_gate` 20/0; `build_tests.sh` 1445/0 + fuzz/negative/differential ok |

### Self-apply log (STEP=3)

```
compiler/out/mlcc --check-only compiler/main.mlc
# exit 0; stderr empty (no warning[W-CYCLE])
```

Control: Shared Parent/Child cycle still emits 2× `warning[W-CYCLE]` (lint not dead).

## Алгоритм

```
1. Построить граф достижимости типов по полям: ребро A→B, если поле A имеет
   тип Shared<B> (или [Shared<B>], или Shared<B> внутри generic).
2. Найти SCC (сильно связные компоненты) — Tarjan, O(V+E).
3. Для каждого SCC размера > 1 (или самопетля): если ни одно ребро цикла
   не идёт через Weak<_> — warning W-CYCLE со span объявления типа.
```

Эвристика по **структуре типов**, не по значениям в рантайме — возможны
ложные срабатывания (структурный цикл типов ≠ обязательный рантайм-цикл
значений), поэтому warning с suppression, не error.

## Файлы

- `compiler/checker/cycle_lint.mlc` — Shared reachability cycle lint
- `compiler/checker/check/check.mlc` — wire + error/warning split + `CheckOut.warnings`
- `compiler/pipeline.mlc` — `emit_checker_warnings` via `eprintln`
- `compiler/checker/diagnostic_codes.mlc` — `W-CYCLE`

## Формат диагностики

```
warning[W-CYCLE]: recursive type cycle involving Parent through Shared
  without Weak on the back edge; may leak. Consider Weak on the back edge.
  --> node.mlc:1:6
```

## Repro

```mlc
type Parent = { children: [Shared<Child>] }
type Child = { parent: Shared<Parent> }   // цикл без Weak — должен дать W-CYCLE

fn main() -> i32 = 0
```

Контрольный случай (не должен срабатывать):

```mlc
type Parent2 = { children: [Shared<Child2>] }
type Child2 = { parent: Weak<Parent2> }   // цикл разорван Weak — без warning
```

## Verify gate (STEP=4 — passed 2026-07-09)

```bash
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
MLC_CXX=g++ compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2 --exclude=obj   # identical
scripts/regression_gate.sh   # 20/0
compiler/tests/build_tests.sh  # 1445/0 + fuzz/negative/differential ok
```
