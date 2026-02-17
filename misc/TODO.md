# MLC Compiler - Status & TODO

## Current Status
- **Tests:** 2577 total (full suite), 0 failures, 0 errors, 2 skips
- **Line Coverage:** 84.27% (branch: 58.82%)
- **C++ AST DSL:** Production ready
- **MLC Language:** Core features implemented, ownership model and async lowering pending
- **Last Updated:** 2026-02-17

## Implemented Features

### Core Language
- Function declarations: `fn name(params) -> RetType = expr`
- If/else expressions, while/for loops, break/continue
- Let bindings (immutable and mutable)
- Product types (records): `type Point = { x: f32, y: f32 }`
- Sum types (ADT): `type Result = Ok(T) | Err(E)` → `std::variant`
- Pattern matching (guards, nested, or-patterns) → `std::visit`
- Generics: `fn identity<T>(x: T) -> T` → C++ templates
- Type constraints: `<T: Numeric>` → C++20 concepts
- Lambda expressions with closure capture analysis
- Module system with imports/exports, header/implementation generation
- Pipe operator: `x |> f |> g`
- List comprehensions: `[x * 2 for x in arr if x > 0]`
- String interpolation: `"Hello, {name}!"`
- Array operations: indexing, slicing, HOF (map, filter, fold, etc.)
- Bitwise operators: `&`, `|`, `^`, `~`, `<<`, `>>`
- Extended int types: u8, u16, u32, u64, i8, i16
- Character literals: `'a'`, `'\n'`
- Spread operator for records: `{ ...base, z: 3 }`
- Block syntax: `fn foo() -> i32 = { let x = 1; x }`

### Traits (Basic)
- Trait declarations: `trait Show { fn show(self) -> str }`
- Trait implementations: `extend Type with Trait { ... }`
- Extension methods: `extend Type { ... }`
- Generic traits: `trait Into<T> { fn into(self) -> T }`
- Operator overloading via traits (Add, Sub, Mul, Div)
- C++ codegen for trait methods

### Optimizations
- ConstantFoldingPass: arithmetic, comparison, boolean, bitwise
- DeadCodeEliminationPass: unreachable code, unused variables

### Smart Pointers (Parsing + Codegen)
- Shared<T> → std::shared_ptr<T>
- Weak<T> → std::weak_ptr<T>
- Owned<T> → std::move semantics

### Infrastructure
- Zeitwerk autoloading
- TypeRegistry / FunctionRegistry (unified type system)
- Container/Context DI pattern
- Rule-based backend (19 expression rules, 10+ statement rules)
- RuntimePolicy for lowering strategy selection
- ErrorCollector / ErrorFormatter with source locations
- CLI: `bin/mlc` (compile, --emit-cpp, --keep-tmp, stdin)

## NOT Implemented (Despite Being Discussed in Docs)

### Ownership / Memory Safety
- No move tracking in semantic analyzer
- No use-after-move detection
- No borrow checking (even in unsafe blocks)
- Handle, Observer, Span pointer types — not implemented
- Borrowing rules from LANGUAGE_STRATEGY.md — not enforced

### Async/Await
- Parsing and AST nodes exist
- **No lowering to C++20 coroutines** — async code does not compile to working C++

### Trait System Gaps
- No dynamic dispatch (dyn Trait)
- No orphan rules
- Associated types — parsed but not fully wired through codegen

### Other Missing
- No FFI / extern declarations
- No tuple types `(i32, str)`
- No type aliases `type UserId = i32`
- No const/constexpr declarations
- No destructuring `let { x, y } = point`
- No re-exports `pub use`
- No macros
- No LSP
- No REPL
- No CI/CD configuration
- No multi-level IR (only AST → SemanticIR → C++ AST)
- Query System — deferred, not needed at current scale

## Priority TODO

### High
1. Move semantics / use-after-move detection (ownership Tier 1)
2. Async lowering to C++20 coroutines (or remove from "working" claims)
3. CI setup (GitHub Actions with rake test_fast)
4. Write 1-2 non-trivial MLC programs (>100 lines) as validation

### Medium
5. Trait bounds in generics (full wiring)
6. Tuple types → std::tuple
7. Type aliases → using
8. Const declarations → constexpr
9. FFI basics (extern "C")
10. Record destructuring → structured bindings

### Low
11. Dynamic dispatch (dyn Trait)
12. LSP (diagnostics only)
13. Re-exports
14. Macros (declarative)

## Test Commands

```bash
rake test_unit      # ~30 sec, unit tests only
rake test_fast      # ~2-3 min, excludes E2E
rake test           # ~20 min, full suite including E2E
rake test_e2e       # E2E only (compile + run binaries)
```
