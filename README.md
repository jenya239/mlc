# MLC

Экспериментальный самохостящийся компилятор. MLC-исходники → C++20.

## Статус (март 2026)

- **Bootstrap достигнут**: `mlcc` компилирует сам себя. Третье поколение бинарника работает корректно.
- **Ruby-компилятор**: 1106 unit-тестов, 0 failures — используется как bootstrap-база.
- **Self-hosted `mlcc`**: компилирует весь `compiler/` из MLC в C++, результат компилируется g++.

## Архитектура

```
MLC source → Lexer/Parser → AST → NameCheck/TypeInfer → Codegen → C++20
```

Два компилятора:

| Компилятор | Путь | Описание |
|------------|------|----------|
| Ruby (bootstrap) | `lib/mlc/` | Полнофункциональный, используется для сборки `mlcc` |
| Self-hosted | `compiler/` | MLC-на-MLC, цель проекта |

## Синтаксис

```mlc
fn area(shape: Shared<Shape>) -> i32 =
  match shape
  | Circle { radius } => radius * radius
  | Rect { width, height } => width * height
  end

fn main() -> i32 = do
  const name = "world"
  println(f"hello {name}")
  let mut count = 0
  while count < 3 do count = count + 1 end
  0
end
```

### Ключевые конструкции

| Конструкция | Синтаксис |
|-------------|-----------|
| Функция | `fn name(args) -> T = expr` |
| Do-блок | `fn f() -> T = do stmts... expr end` |
| Неизменяемая привязка | `const x = expr` |
| Изменяемая привязка | `let mut x = expr` |
| Record-тип | `type Point = Point { x: i32, y: i32 }` |
| Sum-тип | `type Shape = Circle { r: i32 } \| Rect { w: i32, h: i32 }` |
| Pattern matching | `match expr \| Pattern => body end` |
| Record destructuring | `match s \| Circle { radius } => ...` |
| Instance-метод | `extend Type { fn method(self: Type) -> T = ... }` |
| Static-вызов | `Type.method()` |
| Instance-вызов | `obj.method()` → `Type_method(obj)` |
| Массив | `[string]`, `arr.push(x)`, `arr[i]` |
| HashMap | `[string: i32]`, `map.set(k,v)`, `map.get(k)` |
| Shared pointer | `Shared<T>`, `Shared.new(val)` |
| F-string | `f"hello {name}, count={count}"` |
| Импорт | `import { Type, fn } from "./module"` |

### Типы

```
i32  i64  f32  f64  bool  string  unit
[T]              — Array<T> (COW)
[K: V]           — HashMap<K,V> (COW)
Shared<T>        — std::shared_ptr<T>
```

## Команды

```bash
# Unit-тесты (быстро, ~50 сек)
bundle exec ruby -Ilib:test -e "Dir['test/mlc/**/*_test.rb'].each { |f| require_relative f }"

# Собрать mlcc из исходников
compiler/build.sh

# Скомпилировать MLC-программу
compiler/out/mlcc path/to/main.mlc
# → генерирует path/to/out/*.cpp и *.hpp

# Скомпилировать и запустить вручную
g++ -std=c++20 -I runtime/include \
    path/to/out/main.cpp \
    runtime/src/io/io.cpp runtime/src/core/string.cpp \
    -o my_program
```

## Структура проекта

```
compiler/           — self-hosted компилятор (MLC-исходники)
  main.mlc          — точка входа
  ast.mlc           — AST-узлы
  lexer.mlc         — лексер
  parser/           — парсер (exprs, decls, types, preds)
  checker/          — name resolution, type inference, registry
  codegen.mlc       — генерация C++
  out/              — скомпилированный mlcc (бинарник + C++)
lib/mlc/            — Ruby bootstrap-компилятор
runtime/            — C++20 runtime
  include/mlc/      — заголовки (String, Array, HashMap, io)
  src/              — реализации (string.cpp, io.cpp)
test/mlc/           — unit-тесты Ruby-компилятора
```

## Генерируемый C++

Sum-тип и pattern matching:
```mlc
type Shape = Circle { radius: i32 } | Rect { width: i32, height: i32 }

fn area(s: Shared<Shape>) -> i32 =
  match s
  | Circle { radius } => radius * radius
  | Rect { width, height } => width * height
  end
```
→
```cpp
struct Circle { int32_t radius; };
struct Rect   { int32_t width; int32_t height; };
using Shape = std::variant<Circle, Rect>;

int32_t area(std::shared_ptr<Shape> s) noexcept {
    return std::visit(overloaded{
        [&](const Circle& circle) { const auto& radius = circle.radius; return (radius * radius); },
        [&](const Rect& rect)     { const auto& width = rect.width; const auto& height = rect.height; return (width * height); }
    }, (*s));
}
```
