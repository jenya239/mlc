# Query engine (planned)

**Status:** stub — no implementation. Future track: `TRACK_QUERY_ENGINE`.

Parent: [ARCHITECTURE.md](ARCHITECTURE.md) (driver vs core, LSP adapters); [PLAN.md](PLAN.md) Phase 2.8 follow-on.

**Prerequisites:** [TRACK_CLEAN_ARCHITECTURE](agent/TRACK_CLEAN_ARCHITECTURE.md) closed — IR layers, verifiers, pass manager, driver split, architecture lint.

---

## Goal

Demand-driven **queries** over compiler state instead of full recompile per LSP request.

Today LSP (`compiler/lsp/`) re-parses and re-checks source per request. A query engine caches analyses and invalidates only affected facts when the document changes.

```text
adapter (LSP, CLI)  →  QueryEngine  →  analyses (symbols, types, diagnostics)
                              ↓
                     PassManager + preserved analyses
```

---

## Query surface (sketch)

| Query | Consumer | Notes |
|-------|----------|-------|
| `diagnostics_of(source)` | LSP `publishDiagnostics` | incremental invalidation by edit range |
| `definition_at(span)` | go-to-definition | symbol index + import graph |
| `type_at(span)` | hover | typed IR snapshot |
| `references_of(symbol)` | find references | future |
| `completions_at(span)` | completion | future |

Queries are **read-only**; transforms stay in the compile pipeline.

---

## Security invariants

Queries run on untrusted workspace input (editor buffers, paths). Rules mirror existing driver/LSP guards:

| Rule | Rationale |
|------|-----------|
| Source paths via `driver_source_path_is_safe` / `module_path_is_safe` | reject `..`, absolute escape, NUL |
| Bounded work per request | max file size, max import depth, query timeout |
| No arbitrary code execution | queries return data; no `eval` / shell |
| Stable IDs only in API | opaque `NodeId` / `SymbolId` — no raw pointers across requests |
| Cache keys include content hash | stale results must not leak across versions |

New query entry points must add a named `*_is_safe` predicate and unit tests (pattern from CLEAN_ARCHITECTURE STEP=8–11).

---

## Implementation notes (future)

- **Invalidation:** edit → map span to affected modules → rerun minimal pass subset (LLVM preserved-analyses model; stub in `compiler/pass_manager.mlc`).
- **Storage:** arena + intern tables; avoid `Shared<T>` churn on hot query path.
- **Threading:** single-threaded query loop first; concurrency track later.
- **Out of scope:** incremental C++ codegen, distributed cache, arbitrary plugin queries.

---

## Related tracks

| Track | Relationship |
|-------|--------------|
| [TRACK_INCREMENTAL](agent/TRACK_CONCURRENCY.md) (future) | persistent CST / stable node ids |
| [TRACK_CORE_IR](agent/TRACK_CLEAN_ARCHITECTURE.md) STEP=7 sketch | lowering target for typed queries |
| [TRACK_LSP](agent/TRACK_LSP.md) | current adapter; migrate to queries incrementally |

---

## Open questions

1. File-level vs project-level query session (multi-root workspace).
2. Partial parse after edit vs full reparse (parser incrementalism).
3. When to open `TRACK_QUERY_ENGINE` vs extend LSP in place.

Planner: draft `TRACK_QUERY_ENGINE.md` when incremental LSP becomes priority.
