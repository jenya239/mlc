# Track: cycle lint — warning on `Shared` type cycles without `Weak`

Parent: [../PLAN.md](../PLAN.md). Depends on:
[TRACK_LANG_WEAK_SUGAR.md](TRACK_LANG_WEAK_SUGAR.md) (эргономика `Weak` должна
существовать раньше, чем линтер начнёт советовать её использовать). Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #5 (Уровень 1 из
части «память и связность»).

## Status: **open** (STEP=3 done 2026-07-09 — self-apply clean)

**Не полный alias-анализ.** Узкая диагностика уровня lint (см. `docs/MLC.md`
план линтера — `G1 → G2 (минимальные правила) → ...`), не блокирующая ошибка.

### Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | `cycle_lint.mlc` + wire in `check.mlc`; `W-CYCLE`; errors-only fail `check`; tests in `test_checker.mlc` |
| 2 | **done** | `CheckOut.warnings`; `eprintln` in `run_checker_pass`; builtin `eprintln` + `map_builtin`; `--check-only` prints `warning[W-CYCLE]` |
| 3 | **done** | Self-apply: `mlcc --check-only compiler/main.mlc` → **0** `W-CYCLE` / warnings (exit 0). No false positives. (`tests_main.mlc` not a merge entry — path resolve fails; not in scope.) |
| 4 | pending | verify-gate (self-host + regression) + close track |

### Self-apply log (STEP=3)

```
compiler/out/mlcc --check-only compiler/main.mlc
# exit 0; stderr empty (no warning[W-CYCLE])
```

Control: `/tmp/wcycle_repro.mlc` still emits 2× `warning[W-CYCLE]` (lint not dead).

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

- Данные уже есть: `TypeRegistry.adt_index` /
  `AdtIndex.constructor_parameters` — найти точное имя в
  `compiler/checker/registry.mlc` (registry уже строит индекс типов и полей
  при `build_registry`).
- Новый файл: `compiler/checker/cycle_lint.mlc` — отдельный проход после
  `build_registry`, читает `adt_index`, реализует Tarjan на существующих
  `Map`/`[T]` структурах (без внешних библиотек).
- Точка вызова прохода — найти, где сейчас в пайплайне запускаются
  post-registry диагностические проходы (искать вызовы других `*_lint`/
  `*_diagnostics` функций в `compiler/checker/`), подключить туда же.

## Формат диагностики

```
warning[W-CYCLE]: recursive type cycle Parent -> Child -> Parent through Shared
  without Weak on the back edge; may leak. Consider Weak<Parent> on Child.parent.
  --> node.mlc:3:1
```

## Repro

```mlc
type Parent = { children: [Shared<Child>] }
type Child = { parent: Shared<Parent> }   // цикл без Weak — должен дать W-CYCLE

fn main() -> i32 = 0
```

До фикса: компиляция проходит без предупреждений. После: `W-CYCLE` в stderr,
компиляция всё равно успешна (warning, не error).

Контрольный случай (не должен срабатывать):

```mlc
type Parent2 = { children: [Shared<Child2>] }
type Child2 = { parent: Weak<Parent2> }   // цикл разорван Weak — без warning
```

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

Плюс: прогнать линтер на самом `compiler/` (self-application) — убедиться,
что 0 ложных срабатываний на реальном коде проекта (или задокументировать
найденные срабатывания как реальные потенциальные утечки).
