# MLC Compiler Progress Tracker

## Current Status (2026-02-17)

### Test Results
```
Full suite:  2577 runs, 6561 assertions, 0 failures, 0 errors, 2 skips
Fast tests:  2439 runs, 6133 assertions, 0 failures, 0 errors, 2 skips
Unit tests:  2113 runs, 5735 assertions, 0 failures, 0 errors, 0 skips
Line Coverage: 84.27% (14111 / 16745)
Branch Coverage: 58.82% (3349 / 5694)
```
2 skips: `GtkGlSampleTest` (external dependency not available)

### Architecture: SemanticGen Pipeline (STABLE)

```
MLC Source → Lexer → Parser → AST → SemanticIR → C++ AST → C++20 Source
```

**Directory Structure**:
```
lib/mlc/
├── common/                    # Entry point, stdlib, diagnostics, typing utils
│   ├── compiler_api.rb        # Public API (compile, parse, to_cpp)
│   ├── index.rb               # Zeitwerk autoloading setup
│   ├── stdlib/                # MLC stdlib (.mlc files + scanner/resolver)
│   ├── diagnostics/           # ErrorCollector, ErrorFormatter, EventBus
│   ├── typing/                # Predicates, Equivalence, Unify, Generics
│   └── analysis/              # PassManager, BasePass
├── source/                    # Frontend
│   ├── parser/                # Lexer, Parser (recursive descent, mixin modules)
│   └── ast/                   # AST node definitions
├── representations/           # IR
│   └── semantic/gen/          # SemanticIR generation
│       ├── pipeline.rb        # Pipeline orchestration
│       ├── engine.rb          # Rule engine + visitors
│       ├── services/          # 24 services in 8 categories
│       ├── rules/             # Transformation rules
│       ├── reducers/          # Program/Function/Type reducers
│       ├── visitors/          # Expression/Statement visitors
│       └── passes/            # ModuleImportPass, ConstantFolding, DCE
├── registries/                # TypeRegistry, FunctionRegistry
├── backends/cpp/              # C++ code generation
│   ├── codegen.rb             # Main lowering
│   ├── bootstrap.rb           # Rule registration
│   ├── rules/                 # Expression/statement lowering rules
│   ├── services/              # CppAstFactory, TypeMapper
│   └── header_generator.rb    # .hpp/.cpp generation
└── compile_error.rb

lib/cpp_ast/                   # Standalone C++ AST library
├── nodes/                     # C++ AST nodes (45 files)
├── builder/                   # DSL, generator, fluent API
├── lexer/                     # C++ lexer
└── parsers/                   # C++ parser (recursive descent)
```

### Test Progression
- 2025-11-10: 1500 runs, 7 failures (Phase 25 complete)
- 2025-11-19: 1524 runs, 0 failures (Zeitwerk)
- 2025-11-24: 1780 runs, 0 failures (Pure functions extraction)
- 2025-11-28: 2336 runs, 0 failures (Spread operator, block syntax)
- 2026-02-17: 2577 runs, 0 failures, 2 skips (full suite after dependency updates)

---

## Completed Phases

| Phase | Description | Date | Key Result |
|-------|-------------|------|------------|
| 21-22 | Service consolidation | 2025-11 | -189 LOC |
| 23 | Visitor pattern migration | 2025-11-02 | Eliminated 54 transformer.send() |
| 24 | IRGen elimination | 2025-11 | -3200 LOC |
| 25 | Codebase reorganization | 2025-11-10 | -10,963 net lines |
| 27 | Documentation cleanup | 2025-11 | -26,336 lines docs |
| 27.5 | Namespace migrations | 2025-11 | 16 commits |
| 28 | Zeitwerk autoloading | 2025-11-19 | -61% requires |
| 28.5 | Pure functions extraction | 2025-11-24 | +124 tests |
| 29 | Stdlib expansion (array HOF, strings, Option/Result) | 2025-11-23 | Done |
| 30 | Error diagnostics (source locations, recovery) | 2025-11-23 | Done |
| 31.1 | Type constraints for generics | 2025-11-23 | Done |
| 32 | Advanced pattern matching (guards, nested, or) | 2025-11-23 | Done |
| 33 | Optimizations (constant folding, DCE) | 2025-11-30 | 44 tests |
| 34.1 | Module visibility (export) | 2025-11-24 | Done |
| 36 | Low-level primitives (bitwise, u8-u64, char) | 2025-11-29 | 74 tests |
| 37 | Traits basic (trait/extend, generic traits) | 2025-11-29 | 13 tests |
| 38 | Operator overloading via traits | 2025-11-30 | Done |

## Not Implemented

See `misc/TODO.md` for full list. Key gaps:
- Ownership / move semantics / borrow checking
- Async lowering to C++20 coroutines
- FFI
- Tuple types, type aliases, const/constexpr
- CI/CD, LSP, REPL
