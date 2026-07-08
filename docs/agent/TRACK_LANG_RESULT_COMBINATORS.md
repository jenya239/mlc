# Track: Result/Option combinators + `?` auto-conversion via `From`

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §A2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #3.

## Status: **open**

Комбинаторы уже специфицированы в `docs/MLC.md` §A2 (методы, C++-трансляция
через `std::visit`/`holds_alternative` на `std::variant<Ok<T>,Err<E>>`).
Аудит добавляет **новую** часть — авто-конверсия ошибки в `?` через `From`
(не покрыта существующей спецификацией A2).

## Scope

1. **Result методы** (`MLC.md` §A2): `map`, `map_err`, `and_then`, `or_else`,
   `unwrap_or`, `unwrap_or_else`, `ok`.
2. **Option методы** (`MLC.md` §A2): `map`, `and_then`, `or_else`,
   `unwrap_or`, `filter`, `ok_or`.
3. **Новое (этот аудит): `?` с авто-конверсией `E`.** Сейчас `?` работает,
   только если тип ошибки совпадает с типом ошибки функции. Ввести
   `From<E1> for E2` (трейт), и при несовпадении `?` ищет `impl From<E1> for
   E2` в registry вместо прямой ошибки типов.

## Файлы

- **Checker (комбинаторы):** `infer_method_from_object_and_arguments` —
  self-hosted аналог `compiler/checker/infer/infer_operand_combine.mlc:120`
  (`infer_method_from_object_and_arguments`) — добавить ветки для
  `TGeneric("Result", ...)` и `TGeneric("Option", ...)`, выводящие тип
  результата из типа переданной лямбды.
- **Ruby reference (read-only):** проверить, есть ли уже реализация в
  `lib/mlc/` (искать `and_then`/`map_err` в `lib/mlc/registries/`,
  `lib/mlc/backends/cpp/`) — если Ruby уже поддерживает, взять оттуда точную
  семантику один в один, не изобретать заново.
- **Codegen (комбинаторы):** место lowering методов на `Result`/`Option` —
  искать текущую обработку `ExprQuestion` (`?` оператор) в
  `compiler/codegen/` как ориентир по стилю (`std::holds_alternative`
  каскады, не `std::visit` — см. отдельную рекомендацию аудита про
  `std::visit`-баги, не смешивать в этом треке).
- **`?` + `From` (новое):** место, где сейчас проверяется совпадение типа `E`
  для `?` (искать в checker обработку `ExprQuestion` — вероятно рядом с B3
  exhaustive `let`) — при несовпадении не сразу ошибка, а поиск
  `extend E2 : From<E1>` в `TraitRegistry`/`ImplRegistry`
  (`compiler/checker/registry.mlc`).

## Repro

```mlc
fn parse(source: string) -> Result<i32, string> = Ok(42)
fn check(value: i32) -> Result<i32, string> = Ok(value * 2)

fn pipeline(source: string) -> Result<string, string> =
  parse(source)
    .and_then(check)
    .map(v => v.to_string())
    .map_err(e => "pipeline failed: " + e)

fn main() -> i32 = do
  match pipeline("x") {
    Ok(s) => println(s),
    Err(e) => println(e)
  }
  0
end
```

До фикса: `.and_then`/`.map`/`.map_err` — `E001` (метод не найден). После:
exits 0, печатает `84`.

`From`-конверсия — отдельный repro:

```mlc
type ParseError = { message: string }
type AppError = { message: string }

extend AppError : From<ParseError> {
  fn from(e: ParseError) -> AppError = AppError { message: e.message }
}

fn parse(s: string) -> Result<i32, ParseError> = Err(ParseError { message: "bad" })
fn run(s: string) -> Result<i32, AppError> = do
  let value = parse(s)?   // ParseError -> AppError через From, без ручного map_err
  Ok(value)
end
```

До фикса: `?` требует совпадения типов ошибки, `E00X` type mismatch. После:
компилируется, авто-конвертирует через `From_from`.

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

Делать двумя отдельными шагами/коммитами: (1) комбинаторы, (2) `?` + `From` —
независимые изменения в разном коде.
