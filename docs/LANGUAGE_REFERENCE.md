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

Status: pending — filled in STEP=2.

Planned topics:

- `let` / `const` (stmt) / `let mut` / `let const`
- Immutability vs mutation (`.push` / `.set` require `let mut`)

## Functions

Status: pending — filled in STEP=2.

Planned topics:

- `fn name(args) -> RetType = expr` (one-line)
- `fn name(args) -> RetType = do ... end` (multi-line; not `{ ... }`)
- `export fn` for module public API

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
