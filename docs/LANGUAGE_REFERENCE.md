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

Status: pending — filled in STEP=3.

Planned topics:

- Record types (`type Name = { field: Type }`)
- Sum types (`type Name = Variant1(Type) | Variant2`)
- Generics (`type Result<T, E> = Ok(T) | Err(E)`)

## Pattern matching

Status: pending — filled in STEP=3.

Planned topics:

- `match` expressions
- Or-patterns
- Guards

## Traits

Status: pending — filled in STEP=4.

Planned topics:

- `extend Type { ... }` (inherent methods)
- `extend Type : Trait { ... }` (trait impl)
- Orphan rule

## Error handling

Status: pending — filled in STEP=5.

Planned topics:

- `Result` / `Option`
- `?` early-return operator
- `T!E` sugar (if documented as shipping)

## Closures

Status: pending — filled in STEP=6.

Planned topics:

- Closure syntax and capture
- Escape analysis (why) — see
  [archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md](archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md)

## Arrays, maps, strings

Status: pending — filled in STEP=7.

Planned topics:

- `Array` / `Map` / `string`
- Higher-order methods (`map` / `filter` / `fold`)

## Concurrency

Status: pending — filled in STEP=8.

Planned topics:

- Condensed “how to use”: `spawn` / `Mutex` / `Channel` / `scope`
- Canonical accept-loop demo path
- Full spec: [CONCURRENCY_V2.md](CONCURRENCY_V2.md)

## FFI

Status: pending — filled in STEP=9.

Planned topics:

- `extern fn` / `extern type` / `extern lib`
- Safety contract (`unsafe` framing) — see [FFI_LAYER.md](FFI_LAYER.md)
  and related FFI tracks
