# Track: Array HOF (`map`/`filter`/`fold`/...)

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §A1. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #1 (высший приоритет
аудита, совпадает с уже существующим приоритетом №1 проекта).

## Status: **open**

Уже специфицировано в `docs/MLC.md` §A1 (методы, типы, C++-трансляция).
Этот трек — реализационный, не design-трек: перевести спецификацию в код.

**Проблема (подтверждено этим аудитом на реальном коде):** `compiler/` сейчас
514 `while`-циклов (было 210 на момент записи `MLC.md:60` — цифра там
устарела, разрыв только увеличился), 30 `extend`. Без HOF self-hosted
компилятор не может писать себя идиоматично.

## Scope (MVP, по `MLC.md` §A1)

Методы: `map`, `filter`, `fold`, `flat_map`, `any`, `all`, `find`,
`find_index`, `sort_by`, `group_by`, `zip`, `enumerate`, `flat`, `take`,
`drop`, `sum`, `join`. Не обязательно все сразу одним шагом — `map`/`filter`/
`fold` первыми (наибольшее покрытие реальных `while`-циклов по названному в
`MLC.md` анализу: 80%).

## Файлы

- **Ruby reference (read-only):** искать существующую реализацию array-методов
  в `lib/mlc/` (COW `Array` — `lib/mlc/backends/cpp/` codegen для методов
  массива, `lib/mlc/registries/` для сигнатур встроенных методов) — сверить,
  что Ruby-эталон уже поддерживает `map`/`filter`/`fold` на `[T]` (вероятно да,
  раз это A1 уже часть языка); если нет — сначала фиксируется в Ruby, потом в
  mlcc (конвенция проекта).
- **Checker:** `compiler/checker/semantic_type_structure.mlc:369`
  (`builtin_method_return_type`) — добавить ветки для новых методов с выводом
  типа результата из типа лямбды-аргумента (`map<U>` требует вывести `U` из
  `T -> U`).
- **Codegen:** найти, где сейчас lowering методов на `[T]` (искать
  `ArrayType`/`"length"`/`"push"` в `compiler/codegen/` — начать с того же
  файла/функции, что уже обрабатывает встроенные array-методы) — добавить
  `map`→`std::transform`, `filter`→`std::copy_if`, `fold`→ ручной
  accumulate-цикл или `std::accumulate`.
- **Runtime:** проверить, нужен ли новый метод в `mlc::Array<T>` (реестр
  runtime — искать в `runtime/include/mlc/`) или лямбда-based codegen
  достаточно без изменений в runtime-заголовке.

## Repro / verify (шаблон на каждый метод)

```mlc
fn main() -> i32 = do
  let xs = [1, 2, 3, 4]
  let doubled = xs.map(x => x * 2)
  let evens = xs.filter(x => x % 2 == 0)
  let total = xs.fold(0, (acc, x) => acc + x)
  doubled[0] + evens.length() + total
end
```

До фикса: `E001`/`E005` (метод не найден / не типизируется). После: exits 0,
результат `2 + 2 + 10 = 14`.

**Verify gate:**

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

Добавить unit-тесты в `compiler/tests/test_codegen.mlc`/`test_checker.mlc`
(искать существующий паттерн для встроенных методов массива, мержить в тот же
стиль, не изобретать новый).

## Per-turn template

```
| method | <map|filter|fold|...> |
| done | <one line> |
| verify | test_compiler_mlc N/0; diff_exit=0; repro before=fail after=pass |
| next | <следующий метод> |
```
