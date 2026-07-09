# Track: non-escaping closures by default (escape analysis, C++ template codegen)

Parent: [../PLAN.md](../PLAN.md), [../MEMORY_MODEL.md](../MEMORY_MODEL.md).
Source: [../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md), Часть
1b (#2 в приоритизированном списке — наивысшая ценность после Array HOF).
Related, already closed: [TRACK_LAMBDA_CAPTURE.md](TRACK_LAMBDA_CAPTURE.md)
(фиксировал `[&]`→`[=]` для корректности захвата, не про эту оптимизацию —
не пересекается по коду, читать для контекста текущей формы codegen лямбд).

## Status: **open** — STEP=1 done (Ruby escape analysis + template codegen)

**Driver 2026-07-09:** Wired `EscapeAnalyzer` into Ruby pipeline. Non-escaping
`fn(...)` params → `template<typename __FN>` via `Func#synthetic_type_params` /
`Param#template_type_name`. Verify: `closure_escape_analysis_test.rb` 7/0.
Note: live tree is `lib/mlc/representations/` (Zeitwerk); do not commit duplicate `lib/mlc/nxt/`.

## Формальная граница (не размывать в реализации)

Closure со статически выводимым временем жизни (не переживает вызов/scope,
где создана) → анонимная generic-структура, инстанцируется шаблоном, ноль
аллокаций, ноль refcount (как Rust `impl Fn(...)`, как нативные C++20
лямбды). Closure, обязанная пережить создающий вызов непредсказуемо (в поле,
в разнородной коллекции, через границу потока) → математически требует
type erasure (`std::function`/`std::move_only_function`/`shared_ptr`) — этот
случай **остаётся тяжёлым**, инвертируется только дефолт для остальных.

## Правило вывода escaping (Swift `@escaping`-модель, узкий анализ, НЕ alias-анализ)

```
Замыкание ESCAPES, если хотя бы одно:
  (1) присвоено в поле record/сохранено в let, переживающий текущую функцию;
  (2) возвращено из функции (return type содержит Fn(...));
  (3) передано в параметр, помеченный escaping;
  (4) захвачено внутри Spawn (cross-thread, ExprSpawn в ast.mlc).
Иначе — non-escaping.
```

Один бит на замыкание, без lifetime-lattice, без регионов — ровно то, что
делает Swift-анализ практичным.

## Синтаксис

- Параметр-замыкание по умолчанию non-escaping → codegen `template<class F>`
  (ноль RC).
- Escaping-параметр — явный маркер в типе: `handler: escaping Fn(Request) ->
  Response` → `std::move_only_function` (C++23, если проект держит потолок
  C++20 — fallback на `std::function`, пометить явно при реализации какой
  выбран).
- Возврат замыкания (`-> (T -> U)`) — автоматически escaping (правило (2)),
  без явного маркера.

## Класс миграции

**Breaking для codegen** (форма генерируемого C++ для параметров-функций
меняется — существующие бинарники пересобираются иначе), **non-breaking для
языка** (исходный MLC-код не меняется, кроме новых случаев, где нужен явный
`escaping`).

## Порядок реализации (конвенция проекта: Ruby-эталон первым)

### STEP=1 — Ruby-эталон escape-анализа — **done**

**Файлы:** `lib/mlc/representations/semantic/gen/services/features/capture_analyzer.rb`
(использован для текущего `[=]`-фикса в `TRACK_LAMBDA_CAPTURE.md`) — расширить его:
сейчас он решает форму `[=]` vs `[&]` (безопасность), нужно добавить второй,
независимый вывод — escaping-статус (производительность/форма кодогена).
Не путать две задачи в одном проходе, если текущий код их не разделяет уже.

**Результат STEP=1:** Ruby генерирует `template<class F>` для non-escaping
параметров-лямбд, `std::function` для escaping — с тестами на все 4 правила
escaping выше (по одному тесту на правило + один explicit-маркер тест).

### STEP=2 — self-hosted checker (после STEP=1 зелёного)

**Файлы:** новый проход в `compiler/checker/` (например
`escape_analysis.mlc`) — для каждого `ExprLambda` (искать определение в
`compiler/frontend/ast.mlc`) определить escaping-статус по 4 правилам,
используя уже существующие узлы `ExprSpawn`, `StmtReturn`, типы полей из
`compiler/checker/registry.mlc`.

### STEP=3 — self-hosted codegen

**Файлы:** `compiler/codegen/` — точка, где сейчас генерируется сигнатура
функции с параметром-функцией (искать lowering `FunctionType` в параметрах)
— эмитить `template<class F>` для non-escaping, текущую форму — для
escaping.

## Repro

```mlc
// non-escaping: должен стать template<class F>, не std::function
fn apply_twice(f: Fn(i32) -> i32, x: i32) -> i32 = f(f(x))

// escaping: должен остаться std::function/move_only_function
type Handler = { callback: escaping Fn(i32) -> i32 }

fn make_handler(f: escaping Fn(i32) -> i32) -> Handler = Handler { callback: f }
```

Verify через сгенерированный C++: `grep "std::function" out.cpp` не должен
матчить сигнатуру `apply_twice`, должен матчить поле `Handler::callback`.

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

**Дополнительно (это единственный трек аудита, напрямую влияющий на
скорость сборки — измерить, не полагаться на теорию):** после STEP=3,
`time compiler/out/mlcc -o /tmp/... compiler/main.mlc` — сравнить с
ориентиром из `.cursor/rules/mlcc-self-host-verification.mdc` (~4-5s на g++
-O2 до этого изменения), задокументировать разницу.
