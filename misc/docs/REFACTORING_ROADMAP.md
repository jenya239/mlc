# MLC Compiler Refactoring Roadmap

## Overview
Comprehensive architecture refactoring to eliminate code complexity, improve maintainability, and follow Ruby best practices.

## Completed Phases

### ✅ Phase 21: Service Consolidation (6h)
**Status**: COMPLETE (2025-10)
**Results**: Services reduced from 16 → 14 (-12%, -141 LOC)

- Deleted GenericCallResolverService (63 lines) - thin wrapper
- Deleted LambdaTypeInferenceService (78 lines) - duplicate functionality
- Moved stdlib files to services/ for better organization
- Fixed delegate method bugs
- All tests pass ✅

### ✅ Phase 22: Eliminate Delegate Methods (3h)
**Status**: COMPLETE (2025-10)
**Results**: -48 LOC (removed 8 delegate methods)

- Removed 8 delegate methods from IRGen (~50 lines)
- Updated call sites to use direct service access
- Fixed Phase 21 leftover require path issues
- All tests pass ✅

### ✅ Phase 23: Visitor Pattern Migration (20h)
**Status**: COMPLETE (2025-11-02)
**Results**: Eliminated transformer.send() anti-pattern

- Migrated from Rules calling transformer.send() to Visitor Pattern
- Created ExpressionVisitor and StatementVisitor
- Updated all expression and statement rules to use visitors
- Eliminated ~54 transformer.send() calls
- Proper separation of concerns achieved
- See: `docs/visitor_pattern_final_summary.md` (deleted, historical)

### ✅ Phase 24: IRGen Elimination (20h)
**Status**: COMPLETE (2025-11)
**Results**: Legacy IRGen removed, SemanticGen established

- **Phase 24-A**: Removed legacy IRGen classes (~3200 LOC)
  - Deleted `lib/mlc/irgen.rb`, `lib/mlc/irgen/` directory
  - Deleted `lib/mlc/irgen_v2/` directory (78 files)
  - All functionality migrated to SemanticGen

- **Phase 24-B**: Consolidated to SemanticGen architecture
  - Established `lib/mlc/semantic_gen/` as primary transformation pipeline
  - Rules, Services, Visitors, Reducers properly organized
  - All tests pass ✅

### ✅ Phase 25: Codebase Reorganization (40h)
**Status**: COMPLETE (2025-11-10)
**Results**: Clean architecture following Rust/Crystal best practices

Completed in 6 stages (commits d915d0d → 7255af4):

- **ЭТАП 1**: Created directory structure (compiler/, core/, infrastructure/)
- **ЭТАП 1.5**: Moved root files + cleaned up legacy code (78 files deleted)
- **ЭТАП 2**: Moved stdlib utilities to compiler/stdlib/
- **ЭТАП 3**: Reorganized services by functionality (8 categories: builders/, checkers/, features/, imports/, inference/, registration/, scope/, utils/)
- **ЭТАП 4**: Removed duplicate services (3 files, 351 LOC)
- **ЭТАП 5**: Analyzed large classes (TypeChecker, TypeInferenceService) - decided NOT to split due to high cohesion
- **ЭТАП 6**: Finalization - removed empty directories, created documentation

**Statistics**:
- 211 files changed
- +2,363 lines added
- -13,326 lines deleted
- Net reduction: ~11,000 lines
- 0 regressions
- Tests: 1500 runs, 3872 assertions, 7 failures, 0 errors

**Final Structure**:
```
lib/mlc/
├── application.rb          # Entry point
├── semantic_gen.rb         # SemanticGen loader
├── core/                   # Registries (TypeRegistry, FunctionRegistry)
├── infrastructure/         # EventBus, PassManager
├── compiler/               # Cross-phase utilities
│   └── stdlib/            # Stdlib scanning/resolution
├── semantic_gen/          # SemanticIR generation phase
│   ├── services/          # 8 categories (builders, checkers, features, etc.)
│   ├── rules/
│   ├── reducers/
│   ├── visitors/
│   └── passes/
├── analysis/              # Analysis passes
├── backend/               # Code generation (C++ lowering)
│   ├── codegen/
│   ├── metadata_generator.rb
│   └── header_generator.rb
├── type_system/           # Type system utilities
├── rules/                 # Declarative rule layer
└── ...
```

---

## Current Architecture State (2025-11-10)

### Phase-Specific Services
**SemanticGen Services** (organized in semantic_gen/services/):

**Builders** (5):
- IrBuilder - SemanticIR construction
- AstFactory - AST node creation
- TypeBuilder - Type construction
- ArrayLiteralBuilder - Array literal handling
- RecordLiteralBuilder - Record literal handling

**Checkers** (3):
- TypeChecker - Type validation and compatibility
- AstTypeChecker - AST type checking
- SemanticIrClassifier - IR classification

**Features** (5):
- LambdaService - Lambda transformation
- MatchService - Pattern matching
- ListComprehensionService - List comprehensions
- LoopService - Loop handling
- IndexAccessService - Index access

**Imports** (2):
- ImportService - Module imports
- ModuleResolver - Module resolution

**Inference** (1):
- TypeInferenceService - Type inference and resolution (591 LOC, 31 methods)

**Registration** (2):
- FunctionRegistrationService - Function registration
- SumTypeConstructorService - Sum type construction

**Scope** (3):
- VarTypeRegistry - Variable type tracking
- ScopeContextService - Scope management
- ModuleContextService - Module context

**Utils** (3):
- TypeResolutionService - Type resolution
- TypeUnificationService - Type unification
- PurityAnalyzer - Expression purity analysis

### Cross-Phase Components
**Compiler Utilities** (compiler/):
- StdlibScanner - Stdlib module scanning
- StdlibResolver - Module resolution
- StdlibSignatureRegistry - Signature caching

**Core Registries** (core/):
- TypeRegistry - Global type registry
- FunctionRegistry - Global function registry
- FunctionSignature - Function signature representation

**Infrastructure** (infrastructure/):
- EventBus - Event system
- PassManager - Compilation pipeline

---

## Pending Phases

### Phase 26: Root Directory Review (2h, Low)
**Goal**: Review if remaining root files need reorganization

**Current Root Files**:
- application.rb (entry point - keep in root)
- semantic_gen.rb (phase loader - keep in root)

**Analysis**: Root is now clean with only 2 essential files. No action needed.

---

### ✅ Phase 27: Documentation Improvements (4h)
**Status**: COMPLETE (2025-11-10)
**Results**: Documentation cleaned up and modernized

**Tasks Completed**:
1. ✅ Cleanup obsolete documentation (24 files removed total)
   - 22 files: chat logs, completed migrations, obsolete plans
   - 2 files: ARCHITECTURE_GUIDE.md, ARCHITECTURE_REVIEW.md (obsolete)
2. ✅ Update REFACTORING_ROADMAP.md - added Phase 23-25, updated architecture state
3. ✅ Update progress.md - complete rewrite reflecting SemanticGen pipeline

**Deferred**:
- Create README files for major directories (not critical, deferred to future work)

---

### ✅ Phase 27.5: Fix Directory/Namespace Mismatches (8h)
**Status**: COMPLETE (2025-11-16 to 2025-11-17)
**Results**: All namespace mismatches resolved, codebase ready for Zeitwerk

**Comprehensive namespace migrations completed in 16 commits**:

**Initial Registry Migration** (commit 6d2a7ce, 2025-11-15):
- Moved 3 registry files: `lib/mlc/common/registry/` → `lib/mlc/registries/`
- Changed namespace: `MLC::Core::` → `MLC::Registries::`
- Updated 22 files with new references
- Test results: 1524 runs, 4014 assertions, 0 failures ✅

**Low-Priority Namespace Fixes** (11 commits, 2025-11-16):
1. `MLC::Compiler` → `MLC::Common::Stdlib` (scanner, signature_registry)
2. `MLC::Compiler` → `MLC::Common::Stdlib` (resolver)
3. `MLC::Compiler::MetadataLoader` → `MLC::Common::Stdlib::MetadataLoaderService`
4. `MLC::Backend` → `MLC::Backends::Cpp` (39 references, 12 files)
5. `MLC::TypeSystem` → `MLC::Common::Typing` (19 references, 6 files)
6. `StdlibSignatureRegistry` → `SignatureRegistry` (redundant prefix removed)
7. `MetadataLoaderService` → `MetadataLoader` (redundant suffix removed)
8. `StdlibResolver` → `Resolver` (redundant prefix removed)
9. `StdlibScanner` → `Scanner` (redundant prefix removed)
10. `CodeGen` → `Codegen` (CamelCase fix for Zeitwerk)
11. `MLC::Analysis` → `MLC::Common::Analysis`

**Medium-Priority Namespace Fixes** (commit 2c8daf8, 2025-11-16):
- `MLC::Infrastructure::EventBus` → `MLC::Common::Diagnostics::EventBus`
- `MLC::Infrastructure::PassManager` → `MLC::Common::Analysis::PassManager`
- `MLC::Diagnostics` → `MLC::Common::Diagnostics`
- 13 files modified (7 production + 6 test)

**High-Priority Namespace Fixes** (4 commits, 2025-11-17):
1. `MLC::AST` → `MLC::Source::AST` (commits b777ff8, 892fbfc)
   - 256 usages migrated
2. `MLC::Parser` → `MLC::Source::Parser` (commit 0f7693c)
   - 126 usages migrated, 10 parser files wrapped
3. `MLC::Rules` → `MLC::Representations::Semantic::Gen::Services` (commit f9843d1)
   - 4 usages migrated

**Impact**:
- **Total commits**: 16
- **Files modified**: ~50+ files
- **Test stability**: 1524 runs, 4014 assertions, 0 failures, 0 errors across all migrations ✅
- **Namespace categories fixed**: Low (11), Medium (2), High (5)

**Documentation** (commit 9adcc46, 2025-11-17):
- Created `misc/docs/phase28_zeitwerk_analysis.md` with comprehensive migration tracking
- Documented all 15 namespace migration commits with file counts and test results

**Result**: Codebase now has strict file path → namespace correspondence, ready for Zeitwerk autoloading

---

### ✅ Phase 28: Autoloading Infrastructure (8h, Medium)
**Status**: COMPLETE (2025-11-19)
**Results**: Eliminated 67 manual requires using Zeitwerk

**Completed in 4 commits** (8075cf7, 3c5d2b8, f822434, 6a13435):
- **Commit 8075cf7**: refactor: eliminate 52 require_relative statements using Zeitwerk
- **Commit 3c5d2b8**: refactor: remove 9 missed base_rule requires from statement rules
- **Commit f822434**: refactor: remove 5 cross-namespace requires from services container
- **Commit 6a13435**: refactor: remove cross-namespace require from cpp container

**Statistics**:
- **Removed**: 67 require_relative statements (52 + 9 + 5 + 1)
- **Files modified**: 68 total (57 initial + 11 follow-up)
- **Net change**: +10 insertions, -75 deletions
- **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅
- **Requires remaining**: 51 (down from 118 before Phase 28)

**Removals breakdown**:
1. Pipeline & Reducers (4 requires): FunctionReducer, TypeReducer, ProgramReducer, ModuleImportPass
2. Semantic rules (29 requires): BaseRule from all semantic/gen/rules files (20 initial + 9 statement rules)
3. C++ rules (29 requires): ExpressionRule, StatementRule, utilities (ForLoopVariable, BlockComplexityAnalyzer)
4. Analysis passes (3 requires): BasePass from effect_analysis, name_resolution, type_check passes
5. Cross-namespace requires (6 requires):
   - Services container (5): SignatureRegistry, MetadataLoader, MatchAnalyzer, TypeConstraintSolver, GenericCallResolver
   - C++ container (1): SemanticIRTypeChecker

**Documentation added** (4 files):
1. lib/mlc/source/parser/parser.rb - Ruby module include constraints
2. lib/mlc/source/parser/base_parser.rb - Lexer and AST dependencies
3. lib/mlc/source/parser/optimized_parser.rb - Inheritance requirement
4. lib/mlc/representations/semantic/nodes/builder.rb - Circular dependency resolution

**Remaining requires** (documented in previous phases):
- Container files: same-namespace dependencies, initialization order
- Entry points: Zeitwerk ignored files, explicit loading
- Parser modules: Ruby include/inheritance constraints

**Benefits achieved**:
- ✅ Zeitwerk autoloading for most of codebase
- ✅ Automatic class loading on-demand
- ✅ Cleaner architecture with fewer manual dependencies
- ✅ Documented rationale for remaining requires
- ✅ Easier to add new components (no manual requires needed)
- ✅ Standard Ruby convention (Zeitwerk)

---

### Phase 29: Ruby IoC Container (12h, Medium)
**Goal**: Replace manual dependency injection with Dry-container

**Current State**:
- Manual service initialization in Container classes
- Hash-based context in rules
- Difficult to test individual components

**Benefits**:
- Explicit dependencies
- Easy to test (swap dependencies)
- Lifecycle management
- Standard Ruby practice

**Deferred**: Lower priority, current architecture works well

---

### Phase 30: Type System Improvements (TBD, High)
**Goal**: Clean up type system architecture

**Analysis Needed**:
- Review type_system/ components
- Determine if they should move to services/
- Document separation of concerns

**Deferred**: Requires deeper analysis

---

### Phase 31: Analysis Passes Implementation (TBD, High)
**Goal**: Fully implement analysis passes for SemanticIR validation

**Current State**:
- `lib/mlc/common/analysis/type_check_pass.rb` - simplified initial version
  - Only validates basic function structure (external vs body)
  - TODO (line 13): "Fully implement type checking for all SemanticIR node types"
- `lib/mlc/common/analysis/name_resolution_pass.rb` - simplified initial version
  - Only collects top-level declarations (functions, types)
  - TODO (line 14): "Fully implement name resolution with scoping and reference checking"

**Required Work**:

**TypeCheckPass**:
- Type validation for all SemanticIR expressions
- Type inference validation
- Generic type parameter validation
- Function call type checking
- Match expression exhaustiveness
- Type compatibility in assignments

**NameResolutionPass**:
- Nested scope tracking (function-local variables)
- Reference resolution (variable usage → declaration)
- Shadowing detection
- Unused variable detection
- Forward reference validation
- Import/module name resolution

**Benefits**:
- Earlier error detection (before codegen)
- Better error messages with source locations
- Safer code generation
- Foundation for advanced optimizations

**Priority**: Deferred until basic compiler features are stable

---

## Future Optimizations (Deferred)

### Query System (Rust rustc approach)
**When**: Performance becomes an issue
**What**: Demand-driven compilation with automatic memoization
**Effort**: 40h

### Binding System (Crystal approach)
**When**: Type inference becomes too complex
**What**: Observer-based type propagation
**Effort**: 60h

### Multi-level IR (Swift SIL approach)
**When**: Optimization pipeline needs restructuring
**What**: Raw SIL → Canonical SIL transformation
**Effort**: 80h

---

## Summary

**Completed** (Phase 21-28): ~109 hours
- ✅ Phase 21: Service Consolidation (6h)
- ✅ Phase 22: Eliminate Delegate Methods (3h)
- ✅ Phase 23: Visitor Pattern Migration (20h)
- ✅ Phase 24: IRGen Elimination (20h)
- ✅ Phase 25: Codebase Reorganization (40h)
- ✅ Phase 27: Documentation Improvements (4h)
- ✅ Phase 27.5: Fix Directory/Namespace Mismatches (8h)
- ✅ Phase 28: Autoloading Infrastructure (8h) - **COMPLETE 2025-11-19**

**Deferred** (Phase 29-31): TBD
- Phase 29: IoC Container (12h)
- Phase 30: Type System Improvements (TBD)
- Phase 31: Analysis Passes Implementation (TBD)

**Expected Benefits Achieved**:
- ✅ Eliminated legacy IRGen (~3200 LOC)
- ✅ Removed duplicate services (351 LOC)
- ✅ Clean directory structure following best practices
- ✅ Proper separation of concerns (phases, services, utilities)
- ✅ Strict file path → namespace correspondence (Zeitwerk-ready)
- ✅ 16 namespace migration commits (50+ files modified)
- ✅ **Zeitwerk autoloading integrated** - eliminated 67 manual requires (51 remaining)
- ✅ **Documented architecture** - remaining requires explained
- ✅ 0 regressions throughout all changes
- ✅ Test stability: 1524 runs, 4014 assertions, 0 failures, 0 errors
- ✅ 10,963+ net lines of code removed
- ✅ Scalable architecture ready for future development

**Last Updated**: 2025-11-19 (after Phase 28 completion + follow-up cleanups)
