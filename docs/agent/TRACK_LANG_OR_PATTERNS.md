# Track: or-patterns with binding + guard exhaustiveness

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §B2, §E2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #6.

## Status: **open**

Два независимых, но связанных пробела в `match`, оба уже частично
специфицированы (`MLC.md` §B2, §E2). Делать двумя шагами.

## STEP=1 — or-patterns с биндингом — **done** (2026-07-09)

**Проблема (`MLC.md` §B2):** `A | B => expr` работал для unit и для
совпадающих имён биндинга (codegen `expand_or_arms` уже ок: `Circle(3)`→9,
`Square(4)`→16). Разные имена (`Circle(r) | Square(side)`) молча принимались.

**Fix:** `compiler/checker/names.mlc` —
`check_pattern_or_binding_consistency` / `check_or_pattern_alternatives_same_bindings`
в `visit_match`; E083 в `diagnostic_codes.mlc`; тесты в `test_checker.mlc`.
Совпадающие имена — 0 ошибок; mismatch — `error[E083]: all alternatives of an
or-pattern must bind the same names`.

## STEP=2 — guards + корректная exhaustiveness

**Синтаксис (`MLC.md` §E2, чистый сахар — вложенный `if` в ветке):**

```mlc
match score {
  n if n >= 90 => "A",
  n if n >= 75 => "B",
  _            => "F"
}
```

**Action:** реализовать guard-парсинг (если ещё не реализован — проверить
`compiler/frontend/parser/` на предмет `PatternGuard`/аналога перед началом).
**Критично (найдено этим аудитом, не в исходной спецификации E2):**
guard-ветка НЕ должна засчитываться как покрывающая при проверке
exhaustiveness (Rust-семантика) — если единственная ветка для варианта имеет
guard и нет catch-all, checker обязан требовать `_`/`else`. Иначе — дыра в
exhaustiveness: guard может не сработать в рантайме, а checker считал match
исчерпывающим.

**Файл:** exhaustiveness-проверка — искать в `compiler/checker/` (вероятно
рядом с checker match-обработки, использующей `adt_index` для перечисления
вариантов) — добавить правило: ветка с guard не уменьшает множество
непокрытых вариантов.

**Repro (должен требовать catch-all):**

```mlc
type Status = Active | Inactive

fn describe(s: Status) -> string =
  match s {
    Active if true => "on"
    // нет catch-all, нет ветки для Inactive без guard — должна быть ошибка exhaustiveness
  }
```

До фикса (если guard уже реализован без этого правила): ложно считается
exhaustive. После: compile error "non-exhaustive match" с указанием на
`Inactive`.

## Verify gate (оба шага)

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

Добавить тесты в `compiler/tests/test_parser.mlc`/`test_checker.mlc` —
искать существующие `match`/`PatternOr` тесты, мержить в тот же стиль.
