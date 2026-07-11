# MLC

Экспериментальный самохостящийся компилятор. MLC-исходники → C++20.

## Статус (май 2026)

- **Bootstrap**: `compiler/build.sh` — Ruby → C++ для всего `compiler/`, затем сборка (по умолчанию `clang++`, см. `MLC_CXX`) → `compiler/out/mlcc`. Холодный старт с нуля через Ruby (`MLCC_FORCE_RUBY=1`) — ~14 мин; обычный инкрементальный ребилд существующего `mlcc` — секунды (skip при неизменённом содержимом `.mlc`, полная регенерация при правке).
- **447 тестов** self-hosted компилятора (`rake test_compiler_mlc`): они пересобирают **только** `compiler/out/run_tests` через Ruby; **свежий `mlcc` после правок в `compiler/**` — только `compiler/build.sh`**.
- **Самосборка**: точка входа `compiler/main.mlc` с импортами — `compiler/out/mlcc -o <dir> compiler/main.mlc`; контроль корректности кодогена: второй проход бинарём, собранным из выхода первого (`mlcc2`), и **`diff -rq` двух каталогов C++** (ожидается пустой diff). При нехватке `/tmp` задать `TMPDIR` внутри репозитория.
- Ruby-компилятор (`lib/mlc/`): ~1106 unit-тестов — bootstrap и эталон семантики, не удаляется.

## Архитектура

```
MLC source → Lexer → Parser → AST → Checker (name resolution + type inference)
           → SemanticIR → Codegen → C++20
```

Два компилятора:

| Компилятор | Путь | Статус |
|---|---|---|
| Self-hosted (`mlcc`) | `compiler/` | Модульная сборка `compiler/main.mlc` → C++; E2E и двойной проход (mlcc/mlcc2) |
| Ruby (bootstrap) | `lib/mlc/` | Полнофункциональный эталон, не удаляется |

**Временный split фич (2026-07-10):** concurrency (`spawn`/`Mutex`/`Channel`/`block_on`) —
только `mlcc`; `std/net/tcp` и прочий `common/stdlib` — только Ruby. Матрица:
[docs/MLC.md](docs/MLC.md) § «Два пайплайна». Follow-up: stdlib на `mlcc`.

## Быстрый старт

```bash
# Собрать mlcc из исходников
compiler/build.sh
# → compiler/out/mlcc

# Скомпилировать MLC-программу
compiler/out/mlcc path/to/main.mlc
# → path/to/out/main.cpp, main.hpp

# Скомпилировать и запустить
g++ -std=c++20 -I runtime/include \
    path/to/out/main.cpp \
    runtime/src/io/io.cpp runtime/src/core/string.cpp runtime/src/core/profile.cpp \
    -o my_program && ./my_program
```

`build_bin.sh` (внутри `build.sh`): линкер **mold → lld → gold**; PCH (`MLCC_PCH=0` off); **`MLCC_INCREMENTAL=1` default** (stamp → skip mlcc codegen); тесты в `out/tests/`. `MLCC_INCREMENTAL=0` — cold. `compiler/scripts/bench_build.sh`. `MLCC_DEV=1` / `MLCC_OPT=2`. Dev: `clang ccache mold`.

## Синтаксис

```mlc
type Shape = Circle { radius: i32 } | Rect { width: i32, height: i32 }

fn area(shape: Shared<Shape>) -> i32 =
  match shape {
    Circle { radius } => radius * radius,
    Rect { width, height } => width * height
  }

fn main() -> i32 = do
  const name = "world"
  println(`hello ${name}`)
  let mut count = 0
  while count < 3 do count = count + 1 end
  0
end
```

### Ключевые конструкции

| Конструкция | Синтаксис |
|---|---|
| Функция | `fn name(args) -> T = expr` |
| Do-блок | `fn f() -> T = do stmts... expr end` |
| Неизменяемая привязка | `const x = expr` / `let x = expr` |
| Изменяемая привязка | `let mut x = expr` |
| Record-тип | `type Point = Point { x: i32, y: i32 }` |
| Sum-тип | `type Shape = Circle(i32) \| Rect { w: i32, h: i32 }` |
| Pattern matching | `match expr { Pattern => body, ... }` |
| Instance-метод | `extend Type { fn method(self: Type) -> T = ... }` |
| Trait | `trait Eq { fn eq(self: T, other: T) -> bool }` |
| Result / ? | `fn f() -> Result<T, E> = risky_call()?` |
| Массив | `[T]`, `arr.push(x)`, `arr.map(f)` |
| HashMap | `Map<K, V>`, `map.set(k, v)`, `map.get(k)` |
| Shared pointer | `Shared<T>`, `Shared.new(val)` |
| Интерполяция | `` `hello ${name}` `` |
| Импорт | `import { Type, fn_name } from "./module"` |

### Примитивные типы

```
i32  i64  u8  usize  f64  bool  char  string  unit
[T]         — Array<T> (COW)
Map<K,V>    — HashMap<K,V> (COW)
Shared<T>   — std::shared_ptr<T>
```

## Генерируемый C++

MLC sum-тип и pattern matching → `std::variant` + `std::visit`:

```mlc
type Shape = Circle { radius: i32 } | Rect { width: i32, height: i32 }

fn area(s: Shared<Shape>) -> i32 =
  match s {
    Circle { radius } => radius * radius,
    Rect { width, height } => width * height
  }
```

```cpp
struct Circle { int radius; };
struct Rect   { int width; int height; };
using Shape = std::variant<Circle, Rect>;

int area(std::shared_ptr<Shape> s) {
    return std::visit(overloaded{
        [&](const Circle& __v) { const auto& radius = __v.radius; return (radius * radius); },
        [&](const Rect&   __v) { const auto& width = __v.width; const auto& height = __v.height; return (width * height); }
    }, (*s));
}
```

## Структура проекта

```
compiler/           — self-hosted компилятор (~23K строк MLC)
  main.mlc          — точка входа
  lexer.mlc, ast.mlc
  parser/           — exprs, decls, types, predicates
  checker/          — name resolution, type inference, transform → SemanticIR
  codegen/          — генерация C++ из SemanticIR
  cpp/              — C++ AST (для парсинга C++ в тестах)
  tests/            — unit + e2e тесты
  out/              — скомпилированный mlcc (бинарник + артефакты)
lib/mlc/            — Ruby bootstrap-компилятор (эталон, не удаляется)
runtime/            — C++20 runtime (String COW, Array COW, HashMap COW, io)
test/mlc/           — unit-тесты Ruby-компилятора (~1100 тестов)
docs/               — архитектура, план развития, дизайн языка
```

## Тесты

```bash
# Self-hosted компилятор (447 тестов, ~10 мин включая сборку)
bundle exec rake test_compiler_mlc

# Ruby-компилятор (1106 тестов, быстро)
bundle exec rake test_mlc

# Оба
bundle exec rake test_self_hosted_stack

# E2E: 4 программы через mlcc + g++
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc

# Regression: frozen stdout suite + misc/examples compile+link sweep
# (scripts/run_examples_compile_sweep.sh; soft-skip missing glfw/sodium/libpq)
bash scripts/regression_gate.sh
```

### Самопроверка mlcc (после правок checker/codegen)

```bash
export TMPDIR=/path/in/repo/tmp   # при нехватке /tmp

compiler/out/mlcc -o "$TMPDIR/mlc_p1" compiler/main.mlc
g++ -std=c++20 -I "$TMPDIR/mlc_p1" -I runtime/include \
  "$TMPDIR/mlc_p1"/*.cpp runtime/src/io/io.cpp runtime/src/core/string.cpp runtime/src/core/profile.cpp -O2 -o "$TMPDIR/mlcc2"
"$TMPDIR/mlcc2" -o "$TMPDIR/mlc_p2" compiler/main.mlc
diff -rq "$TMPDIR/mlc_p1" "$TMPDIR/mlc_p2"   # должно быть пусто
```

## Roadmap

`docs/PLAN.md` — архитектурный план, целевая структура файлов, паттерны, Reddit-презентация.

## Community demo

Reproducible commands: `scripts/reddit_demo.sh` (`--run`, `--record-baseline`).

Media checklist (screenshot / asciinema): [docs/agent/REDDIT_DEMO_MEDIA.md](docs/agent/REDDIT_DEMO_MEDIA.md). Baseline numbers: [docs/agent/reddit_demo_baseline.txt](docs/agent/reddit_demo_baseline.txt).
