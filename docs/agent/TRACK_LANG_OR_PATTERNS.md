# Track: or-patterns with binding + guard exhaustiveness

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §B2, §E2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #6.

## Status: **open**

Два независимых, но связанных пробела в `match`, оба уже частично
специфицированы (`MLC.md` §B2, §E2). Делать двумя шагами.

## STEP=1 — or-patterns с биндингом

**Проблема (`MLC.md` §B2):** `A | B => expr` работает только для
unit-вариантов. `Circle(r) | Square(r) => r * r * PI` не работает.

**Файл:** `compiler/checker/names.mlc:34`
(`collect_pattern_or_bindings(alternatives: [Shared<Pattern>], bindings_so_far:
[string]) -> [string]`) — сейчас (строка 55) рекурсивно берёт биндинги
только из `alternatives[0]`, не проверяя совпадение с остальными
альтернативами.

**Action:** для каждой альтернативы после первой — собрать её биндинги тем
же способом и сверить множество имён (и, если типы не гарантированы
идентичными структурой варианта — типы) с `bindings_so_far`. Несовпадение —
diagnostic error ("all alternatives of an or-pattern must bind the same
names"), не silent fallback. Codegen — общий блок для всех веток, биндинг
берётся из того варианта, который фактически совпал (сейчас, вероятно, уже
работает для унарных вариантов раздельно — проверить, не предполагать).

**Repro:**

```mlc
type Shape = Circle(f64) | Square(f64) | Triangle(f64, f64)

fn area(s: Shape) -> f64 =
  match s {
    Circle(r) | Square(r) => r * r,
    Triangle(a, b) => a * b / 2.0
  }
```

До фикса: parse/checker ошибка на разных именах биндинга между альтернативами
(или неверный codegen, если типы совпадают — сверить фактическое текущее
поведение перед фиксом, не предполагать). После: `area(Circle(3.0))` == `9.0`.

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
