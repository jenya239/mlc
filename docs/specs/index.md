# Spec index (on-demand loading)

Load only rows whose **tags** match the active track. Agent turn workflow stays in [../agent/CONTINUITY.md](../agent/CONTINUITY.md) and [../agent/DEVELOPMENT.md](../agent/DEVELOPMENT.md).

## Product specs

| Document | Tags | Load when |
|----------|------|-----------|
| [../PLAN.md](../PLAN.md) | roadmap, architecture, phase | Planner, new track, north-star scope |
| [../MLC.md](../MLC.md) | language, syntax, semantics, overview | Language features, syntax changes |
| [../LANGUAGE_DESIGN.md](../LANGUAGE_DESIGN.md) | language, design, principles, planned | New language constructs, design rationale |
| [../mlc2.md](../mlc2.md) | parity, ruby, mlcc, checker, codegen | Ruby ⇄ mlcc parity, edge cases |
| [../MEMORY_MODEL.md](../MEMORY_MODEL.md) | memory, runtime, cow, ownership | Types, collections, capture, resources |
| [../RUNTIME_OWNERSHIP_GUIDE.md](../RUNTIME_OWNERSHIP_GUIDE.md) | runtime, ownership, string, array, map | Heap types, refcount, COW implementation |
| [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md) | parser, cpp, import, roundtrip | C++ header import, `cpp_ast`, trivia |
| [../BINDINGS_SEMANTICS_MRFC.md](../BINDINGS_SEMANTICS_MRFC.md) | bindings, let, migration, draft | Future `let`/`const` semantics (no compiler change) |
| [../CODE_REVIEW_2026_06.md](../CODE_REVIEW_2026_06.md) | audit, compiler, structure, debt | Large refactors, folder layout, hotspots |
| [../LARGE_CONTEXT_TASKS.md](../LARGE_CONTEXT_TASKS.md) | agent, external-model, batch, mlc-support | Deciding between batch LLM request vs iterative TDD for a track |

## Verification

| Document | Tags | Load when |
|----------|------|-----------|
| [../../specs/regression/README.md](../../specs/regression/README.md) | regression, gate, verify, stdout | Track close on `compiler/**`, baseline refresh |
| [../agent/TRACK_LOOP_CONTRACTS.md](../agent/TRACK_LOOP_CONTRACTS.md) | agent, gate, regression, loop | Regression gate setup, agent loop contracts |

## Tag → track type (quick map)

| Track touches | Primary tags |
|---------------|--------------|
| `compiler/checker/`, types, infer | `parity`, `language`, `memory` |
| `compiler/parser/`, `compiler/lexer/` | `language`, `parser` |
| `compiler/codegen/`, `compiler/cpp/` | `parity`, `memory`, `runtime`, `cpp` |
| `runtime/` | `memory`, `runtime`, `ownership` |
| Language feature (PLAN §2.x) | `language`, `design`, `mlc2` |
| Agent infra only | `agent` (skip product specs unless needed) |
