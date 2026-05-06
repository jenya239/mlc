# Bindings semantics migration (MRFC draft)

## Motivation and non-goals

Document a possible future migration of `let` / `let mut` / `const` semantics for readability or parity with other languages. This file does not change the compiler; it records scope and migration approach only.

## Before / after (hypothetical)

| Construct | Current | Proposed (example only) |
|-----------|---------|-------------------------|
| Immutable binding | `let x = …` | unchanged or spelled `let const` |
| Mutable binding | `let mut x` | `let x` mutable with explicit `const` for freeze |
| Reassignment | Only with `let mut` | Product-specific |

## Layered impact

- Parser: `lib/mlc/source/parser/`
- Semantic passes: `lib/mlc/representations/semantic/gen/`
- C++ lowering / move rules: `lib/mlc/backends/cpp/`
- Self-hosted parity: `compiler/parser/`, `compiler/checker/`

## Phases

1. Specification and minimal conformance tests.
2. Ruby compiler implementation behind explicit milestone or branch.
3. Mechanical migration of `compiler/**/*.mlc` using AST-based tooling (not plain-text sed).
4. mlcc parity after Ruby baseline is stable.

## Readiness checks

- `rake test_mlc`
- `rake test_compiler_mlc`
- Selected integration / E2E without regressions

## Risks

- Ruby vs mlcc divergence until parity.
- Silent semantic bugs from textual codemods; prefer AST visitors on `MLC::Source::AST::*`.

## Tools

- Parse `.mlc` → AST → transform → emit declarations only where needed.
- Golden tests and diagnostic counts before/after.
