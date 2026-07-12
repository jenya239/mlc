# MLC Language Reference

Parent: [PLAN.md](PLAN.md) §27; track:
[agent/TRACK_LANG_DOCS.md](agent/TRACK_LANG_DOCS.md).

This is a **language reference**, not a tutorial. Each section will hold:
syntax, a short semantics note, and one runnable example taken from an
existing `compiler/tests/e2e/` or `misc/examples/` fixture (path cited —
examples are never invented for this document). Stdlib module APIs belong
in [TRACK_STDLIB_DOCS](agent/TRACK_STDLIB_DOCS.md), not here.

## Contents

- [Bindings](#bindings)
- [Functions](#functions)
- [Types](#types)
- [Pattern matching](#pattern-matching)
- [Traits](#traits)
- [Error handling](#error-handling)
- [Closures](#closures)
- [Arrays, maps, strings](#arrays-maps-strings)
- [Concurrency](#concurrency)
- [FFI](#ffi)

---

## Bindings

Immutable and mutable name bindings. `const` and `let` introduce immutable
bindings; `let mut` is required for assignment and for mutating methods such
as `.push` / `.set` on that binding. Compile-time `let const` is out of scope
for this section until a dedicated e2e fixture is cited.

### `const` (immutable)

Source: [`compiler/tests/e2e/record_update.mlc`](../compiler/tests/e2e/record_update.mlc)

```mlc
fn main() -> i32 = do
  const origin = Point { x: 0, y: 0 }
  const moved = move_x(origin, 5)
  const moved_again = Point { ...moved, y: 3 }
  println(origin.to_string())
  println(moved.to_string())
  println(moved_again.to_string())
  0
end
```

(`Point` / `move_x` are defined earlier in the same file.)

### `let` (immutable)

Source: [`compiler/tests/e2e/spawn_side_effect.mlc`](../compiler/tests/e2e/spawn_side_effect.mlc)

```mlc
fn main() -> i32 = do
  let task_a = spawn do side() end
  block_on(task_a)
end
```

### `let mut` (mutable)

No e2e fixture uses `let mut` yet. Source (excerpt from `main`):
[`misc/examples/loops_demo.mlc`](../misc/examples/loops_demo.mlc)

```mlc
  let numbers = [1, 2, 3, 4, 5]
  let mut sum = 0
  for n in numbers do
    sum = sum + n
  end
  println("For loop sum [1..5]: " + sum.to_string())
```

## Functions

Functions are declared with `fn`. A one-line body uses `= expr`; a multi-line
body uses `= do ... end` (not `{ ... }`). Parameters may carry types; the
return type follows `->`.

### One-line body

Source: [`compiler/tests/e2e/fibonacci.mlc`](../compiler/tests/e2e/fibonacci.mlc)

```mlc
fn fib(n: i32) -> i32 =
  if n <= 1 then n else fib(n - 1) + fib(n - 2) end
```

### Multi-line `do` / `end`

Source: [`compiler/tests/e2e/hello.mlc`](../compiler/tests/e2e/hello.mlc)

```mlc
fn main() -> i32 = do
  println('hello world')
  0
end
```

### `export fn` (module public API)

Not used in `compiler/tests/e2e/`. Source:
[`compiler/tests/support/golden_harness.mlc`](../compiler/tests/support/golden_harness.mlc)

```mlc
export fn golden_relative_path_is_safe(relative_path: string) -> bool = do
  if !test_relative_path_is_safe(relative_path) then
    false
  else if relative_path.length() < 16 then
    false
  else if relative_path.substring(0, 16) != "fixtures/golden/" then
    false
  else
    true
  end
end
```

(`test_relative_path_is_safe` is imported in the same file; `export` marks the
function as the module’s public surface for importers.)

## Types

Nominal type declarations: records (product), sums (variants), and generics.

### Record

Source: [`compiler/tests/e2e/record_update.mlc`](../compiler/tests/e2e/record_update.mlc)

```mlc
type Point = { x: i32, y: i32 }

fn move_x(p: Point, dx: i32) -> Point = Point { ...p, x: p.x + dx }
```

(Field update uses `Point { ...p, x: ... }` — see the same file’s `main`.)

### Sum

Source: [`compiler/tests/e2e/sum_types.mlc`](../compiler/tests/e2e/sum_types.mlc)

```mlc
type Shape = Circle(i32) | Rect(i32, i32)
```

Variants are constructed as `Circle(5)` / `Rect(3, 4)` in that file’s `main`.

### Generic sum

Source: [`compiler/tests/e2e/result.mlc`](../compiler/tests/e2e/result.mlc)

```mlc
type Result<T, E> = Ok(T) | Err(E)
```

## Pattern matching

`match` selects an arm by pattern. Wildcards use `_`. Guards use `if` on an
arm. Or-patterns share one body across alternatives (same bound names).

### `match` (sum variants)

Source: [`compiler/tests/e2e/sum_types.mlc`](../compiler/tests/e2e/sum_types.mlc)

```mlc
fn area(s: Shape) -> i32 =
  match s {
    Circle(r) => r * r * 3,
    Rect(w, h) => w * h
  }
```

### Guards

Source: [`compiler/tests/e2e/match_guard.mlc`](../compiler/tests/e2e/match_guard.mlc)

```mlc
type Answer = Yes(i32) | No

fn pick(answer: Answer) -> i32 =
  match answer {
    Yes(n) if n > 0 => n,
    Yes(_) => 0,
    No => -1
  }
```

### Or-patterns

No standalone e2e file. Source (program string under test):
[`compiler/tests/test_mir_vm_smoke.mlc`](../compiler/tests/test_mir_vm_smoke.mlc)

```mlc
type Choice = A(i32) | B(i32) | C
fn main() -> i32 = match A(3) {
  A(x) | B(x) => x
  C => 0
}
```

(Alternatives in an or-pattern must bind the same names — see checker E083
in `compiler/tests/test_checker.mlc`.)

## Traits

Traits declare method requirements. `extend Type { ... }` adds inherent
methods. `extend Type : Trait { ... }` implements a trait for a type.
Trait bounds appear on generics (`T: Display`) or as trait-typed parameters.

### Inherent `extend`

Source: [`compiler/tests/e2e/record_update.mlc`](../compiler/tests/e2e/record_update.mlc)

```mlc
type Point = { x: i32, y: i32 }

extend Point {
  fn to_string(self: Point) -> string =
    "(" + self.x.to_string() + ', ' + self.y.to_string() + ")"
}
```

### Trait declaration + `extend Type : Trait`

Source: [`compiler/tests/e2e/trait_e2e.mlc`](../compiler/tests/e2e/trait_e2e.mlc)

```mlc
type Display { fn to_string(self: Self) -> string }

extend i32 : Display {
  fn to_string(self: i32) -> string = extern
}

fn id<T: Display>(x: T) -> T = x
```

### Trait as parameter type

Source: [`compiler/tests/e2e/trait_as_param.mlc`](../compiler/tests/e2e/trait_as_param.mlc)

```mlc
fn f(x: Display) -> unit = ()

fn main() -> i32 = do
  f(42)
  println("ok")
  0
end
```

(`Display` / `extend i32 : Display` are the same as in `trait_e2e.mlc`.)

### Orphan rule

`extend T : Trait` is allowed only in the module that defines **T** or
**Trait** (diagnostic E086). Bare `extend T` (inherent methods) is unchanged.
Details:
[archive/tracks/TRACK_LANG_ORPHAN_RULE.md](archive/tracks/TRACK_LANG_ORPHAN_RULE.md).

## Error handling

Fallible work uses `Result<T, E>` (or the `T!E` sugar for the same type).
`?` unwraps `Ok` or early-returns `Err` from the enclosing function. `Option`
is a sum for optional values (`Some` / `None`).

### `Result` + `?`

Source: [`compiler/tests/e2e/result.mlc`](../compiler/tests/e2e/result.mlc)

```mlc
type Result<T, E> = Ok(T) | Err(E)

fn safe_div(a: i32, b: i32) -> Result<i32, string> =
  if b == 0 then Err('division by zero') else Ok(a / b) end

fn compute(divisor: i32) -> Result<i32, string> =
  Ok(safe_div(10, divisor)? + 1)
```

Callers typically `match` on `Ok` / `Err` (see `main` in the same file).

### `T!E` sugar

`T!E` desugars to `Result<T, E>` in type position. Source:
[`compiler/tests/fixtures/error_union_sugar/with_question.mlc`](../compiler/tests/fixtures/error_union_sugar/with_question.mlc)

```mlc
type Result<T, E> = Ok(T) | Err(E)

fn safe_div(a: i32, b: i32) -> i32!string =
  if b == 0 then Err('division by zero') else Ok(a / b) end

fn compute(divisor: i32) -> i32!string =
  Ok(safe_div(10, divisor)? + 1)
```

Track: [archive/tracks/TRACK_LANG_ERROR_UNION.md](archive/tracks/TRACK_LANG_ERROR_UNION.md).

### `Option`

No e2e fixture. Source: [`misc/examples/vm_option.mlc`](../misc/examples/vm_option.mlc)

```mlc
type Option = Some(i32) | None

fn unwrap_option(value: Option) -> i32 = match value {
  Some(x) => x
  None => 0
}
```

## Closures

Anonymous functions use `(params) => body` or the short form `x => body`.
They capture by value (`[=]` in codegen). Non-escaping closures (do not outlive
the call that creates them) can be lowered without `std::function` type
erasure — see escape analysis.

### Binding a closure

Source: [`misc/examples/vm_lambda.mlc`](../misc/examples/vm_lambda.mlc)

```mlc
fn main() -> i32 = do
  const add = (x: i32, y: i32) => x + y
  add(3, 4)
end
```

### Immediate call

Source: [`misc/examples/vm_lambda_immediate.mlc`](../misc/examples/vm_lambda_immediate.mlc)

```mlc
fn main() -> i32 = ((x: i32) => x + 1)(6)
```

### Passed to array HOF (typically non-escaping)

Source: [`misc/examples/array_hof_demo.mlc`](../misc/examples/array_hof_demo.mlc)

```mlc
  let doubled = numbers.map(x => x * 2)
  let evens = more.filter(x => x % 2 == 0)
```

(Excerpts from `main` in that file.)

### Escape analysis

Closures that must outlive their creating scope (stored in a field, sent across
threads, put in a heterogeneous collection) stay type-erased. Closures that
statically do not escape can use a cheaper C++ template lowering. Full rule and
codegen notes:
[archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md](archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md).

## Arrays, maps, strings

`[T]` is an array (COW `mlc::Array`). `Map<K, V>` is a hash map (COW
`mlc::HashMap`). `string` is the language string type. Mutating methods
(`.push` / `.set`) need a `let mut` binding.

### Array type and length

Source: [`compiler/tests/e2e/empty_array.mlc`](../compiler/tests/e2e/empty_array.mlc)

```mlc
fn count(items: [i32]) -> i32 = items.length()

fn main() -> i32 = do
  println(count([]).to_string())
  0
end
```

### Array HOF (`map` / `filter` / `fold`)

Source: [`misc/examples/array_hof_demo.mlc`](../misc/examples/array_hof_demo.mlc)

```mlc
  let numbers = [1, 2, 3, 4, 5]
  let doubled = numbers.map(x => x * 2)
  let more = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
  let evens = more.filter(x => x % 2 == 0)
  let sum = numbers.fold(0, (acc, x) => acc + x)
  let composed = more
    .filter(x => x % 2 == 0)
    .map(y => y * y)
    .fold(0, (acc, z) => acc + z)
```

(Excerpts from `main` in that file.)

### Map

Source: [`misc/examples/vm_map.mlc`](../misc/examples/vm_map.mlc)

```mlc
fn main() -> i32 = do
  let mut table: Map<string, i32> = Map.new()
  table.set("k", 3)
  table.set("m", 4)
  table.get("k") + table.get("m")
end
```

### String

Source: [`misc/examples/string_operations_demo.mlc`](../misc/examples/string_operations_demo.mlc)

```mlc
  let greeting = "Hello" + ", " + "World" + "!"
  let text = "MiXeD CaSe TeXt"
  println("Upper: \"" + text.upper() + "\"")
  println("Lower: \"" + text.lower() + "\"")
  let padded = "   spaces around   "
  let trimmed = padded.trim()
```

(Excerpts from `main`; the file also covers `contains` / `starts_with` /
`ends_with` / `is_empty`.)

## Concurrency

Structured concurrency: prefer `scope |s| { s.spawn … }` so children are
joined when the scope ends. Bare `spawn` + `block_on` still exists for simple
cases. Full design (Send/Sync, channels, cancellation, supervisors):
[CONCURRENCY_V2.md](CONCURRENCY_V2.md).

### `spawn` + `block_on`

Source: [`compiler/tests/e2e/spawn_side_effect.mlc`](../compiler/tests/e2e/spawn_side_effect.mlc)

```mlc
fn main() -> i32 = do
  let task_a = spawn do side() end
  block_on(task_a)
end
```

### `scope` + `spawn` (parallel children)

Source: [`compiler/tests/e2e/scope_parallel_sleep.mlc`](../compiler/tests/e2e/scope_parallel_sleep.mlc)

```mlc
fn main() -> i32 = do
  scope |task_scope| do
    task_scope.spawn do slow_work(1) end
    task_scope.spawn do slow_work(2) end
  end
  0
end
```

(Both children overlap in wall time; see file comments / gate.)

### Canonical accept loop

Bounded accept + per-connection `scope.spawn` (not bare detached `spawn`).
Source: [`misc/examples/http_scope_accept_loop_demo.mlc`](../misc/examples/http_scope_accept_loop_demo.mlc)

```mlc
  scope |task_scope| do
    while remaining > 0 do
      let stream_option = accept(listener)
      if !stream_option.is_some() then
        accept_failed = 1
        remaining = 0
      else
        let stream = stream_option.unwrap()
        task_scope.spawn do handle_one(stream) end
        remaining = remaining - 1
      end
    end
  end
```

(Excerpt from `main`; Tcp/`HttpServer` imports and `handle_one` are in the
same file.)

### `Mutex` / `Channel`

No e2e demo yet. Language surface is checked in unit tests; runtime lives under
`runtime/include/mlc/concurrency/`. Example program string from
[`compiler/tests/test_mutex_syntax.mlc`](../compiler/tests/test_mutex_syntax.mlc):

```mlc
fn main() -> i32 = do
  let guarded = Mutex.new([])
  guarded.lock(values => do
    values.push(7)
    values.length()
  end)
end
```

Channels, TaskScope cancellation, Isolate, and Supervisor: see
[CONCURRENCY_V2.md](CONCURRENCY_V2.md) and closed tracks under
`docs/archive/tracks/TRACK_CONCURRENCY_*`.

## FFI

Import C/C++ symbols with `extern fn` / `extern type` / `extern lib`. Full-form
bindings name the C symbol and header; optional concurrency attributes
(`blocking` / `thread_safe` / `thread_affine`) apply when `from "<header>"` is
present. There is **no** `unsafe` keyword (Decision B). Spec:
[FFI_LAYER.md](FFI_LAYER.md); safety contract track:
[archive/tracks/TRACK_FFI_SAFETY.md](archive/tracks/TRACK_FFI_SAFETY.md).

### `extern fn` (full form)

Source: [`compiler/tests/e2e/scope_parallel_sleep.mlc`](../compiler/tests/e2e/scope_parallel_sleep.mlc)

```mlc
extern fn sleep_ms_probe(milliseconds: i32) -> i32
  = "mlc::concurrency::test_sleep_ms" from "mlc/concurrency/test_sleep.hpp"
```

### `export extern fn` + `blocking`

Source: [`lib/mlc/common/stdlib/env/env.mlc`](../lib/mlc/common/stdlib/env/env.mlc)

```mlc
export extern fn get(key: string) -> Option<string>
  = "mlc::env::get" from "mlc/env/env_abi.hpp" blocking
```

(Tcp uses the same pattern — see `lib/mlc/common/stdlib/net/tcp.mlc`.)

### `extern type`

Source: [`compiler/tests/fixtures/extern_concurrency_attr/missing_type.mlc`](../compiler/tests/fixtures/extern_concurrency_attr/missing_type.mlc)

```mlc
extern type Handle = "Handle" from "<stdio.h>"
```

### `extern lib`

Declares a link library (`-lname` via `build_bin.sh`). Documented in
[FFI_LAYER.md](FFI_LAYER.md) §2.3; example form:

```mlc
extern lib "pq"
```

### Safety contract (no `unsafe` syntax)

| Concern | Who | Notes |
|---------|-----|-------|
| Concurrency attrs on full-form `extern` | Checker | `W-EXTERN-ATTR` if `from` present without `blocking`/`thread_safe`/`thread_affine` |
| Arity vs imported header stub | Checker | `W-EXTERN-ARITY` when header import is available |
| `RawPointer[T]` lifetime / null | Author | No borrow checker; pointer valid while C object lives |
| Shorthand `extern fn` (no `from`) | — | Marker only; cannot carry concurrency attrs |

Details: [FFI_LAYER.md](FFI_LAYER.md) §9; track
[archive/tracks/TRACK_FFI_SAFETY.md](archive/tracks/TRACK_FFI_SAFETY.md).
