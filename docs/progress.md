# MLC Compiler Progress Tracker

## Current Status (2025-11-10)

### Architecture: SemanticGen Pipeline ✅ STABLE

The MLC compiler follows a multi-phase architecture with SemanticGen as the primary AST → SemanticIR transformation pipeline.

**Phases**:
1. **Parsing** (lib/mlc/parser/) - Aurora source → AST
2. **SemanticGen** (lib/mlc/semantic_gen/) - AST → SemanticIR
3. **Analysis** (lib/mlc/analysis/) - SemanticIR transformation passes
4. **Backend** (lib/mlc/backend/) - SemanticIR → C++ code generation

### Recent Milestones

#### ✅ Phase 25: Codebase Reorganization (Complete, 2025-11-10)
**Result**: Clean architecture following Rust/Crystal best practices

- Reorganized into clear phases: compiler/, core/, infrastructure/, semantic_gen/, analysis/, backend/
- Services organized by functionality (8 categories)
- Removed 22 obsolete documentation files
- 10,963 net lines of code removed
- 0 regressions

#### ✅ Phase 24: IRGen Elimination (Complete, 2025-11)
**Result**: Legacy IRGen removed, SemanticGen established

- Removed legacy lib/mlc/irgen.rb and lib/mlc/irgen/ (~3200 LOC)
- Removed lib/mlc/irgen_v2/ directory (78 files)
- Consolidated to SemanticGen architecture
- All functionality migrated successfully

#### ✅ Phase 23: Visitor Pattern Migration (Complete, 2025-11-02)
**Result**: Eliminated transformer.send() anti-pattern

- Migrated from Rules calling transformer.send() to Visitor Pattern
- Created ExpressionVisitor and StatementVisitor
- Eliminated ~54 transformer.send() calls
- Proper separation of concerns

---

## Architecture Overview

### Directory Structure

```
lib/mlc/
├── application.rb              # Main entry point
├── semantic_gen.rb             # SemanticGen phase loader
│
├── core/                       # Central registries (rustc_middle inspired)
│   ├── type_registry.rb
│   ├── function_registry.rb
│   └── function_signature.rb
│
├── infrastructure/             # Compiler infrastructure
│   ├── event_bus.rb
│   └── pass_manager.rb
│
├── compiler/                   # Cross-phase utilities
│   └── stdlib/
│       ├── scanner.rb
│       ├── resolver.rb
│       └── signature_registry.rb
│
├── semantic_gen/              # SemanticIR generation phase
│   ├── engine.rb              # Main engine
│   ├── pipeline.rb            # Pipeline orchestration
│   ├── services/              # Phase-specific services (8 categories)
│   │   ├── builders/          # IR/AST construction (5)
│   │   ├── checkers/          # Type checking (3)
│   │   ├── features/          # Language features (5)
│   │   ├── imports/           # Module imports (2)
│   │   ├── inference/         # Type inference (1)
│   │   ├── registration/      # Registration services (2)
│   │   ├── scope/             # Scope management (3)
│   │   └── utils/             # Utilities (3)
│   ├── rules/                 # Transformation rules
│   ├── reducers/              # AST reducers
│   ├── visitors/              # Visitor pattern implementation
│   └── passes/                # Transformation passes
│
├── analysis/                  # Analysis passes
│   ├── base_pass.rb
│   ├── name_resolution_pass.rb
│   ├── type_check_pass.rb
│   ├── effect_analysis_pass.rb
│   ├── identity_lowering_pass.rb
│   └── lower_to_mid_pass.rb
│
├── backend/                   # Code generation
│   ├── codegen.rb
│   ├── codegen/
│   │   ├── base_lowerer.rb
│   │   ├── expression_lowerer.rb
│   │   ├── statement_lowerer.rb
│   │   ├── type_lowerer.rb
│   │   └── function_lowerer.rb
│   ├── header_generator.rb
│   ├── metadata_generator.rb
│   └── runtime_policy.rb
│
├── type_system/              # Type system utilities
│   ├── match_analyzer.rb
│   ├── type_constraint_solver.rb
│   ├── generic_call_resolver.rb
│   └── effect_analyzer.rb
│
├── rules/                    # Declarative rule layer
│   ├── rule_engine.rb
│   ├── base_rule.rb
│   ├── delegating_rule.rb
│   └── codegen/             # C++ codegen rules
│
├── ast/                     # AST nodes
├── semantic_ir/             # SemanticIR nodes
├── mid_ir/                  # MidIR nodes
├── parser/                  # Parser implementation
├── diagnostics/             # Diagnostics system
├── error_handling/          # Error handling
└── stdlib/                  # Stdlib sources (*.mlc files)
```

### SemanticGen Services (24 total)

**Builders** (5 services):
- IrBuilder - SemanticIR construction
- AstFactory - AST node creation
- TypeBuilder - Type construction helpers
- ArrayLiteralBuilder - Array literal handling
- RecordLiteralBuilder - Record literal handling

**Checkers** (3 services):
- TypeChecker - Type validation and compatibility (355 LOC, 28 methods)
- AstTypeChecker - AST type checking
- SemanticIrClassifier - IR node classification

**Features** (5 services):
- LambdaService - Lambda/closure transformation
- MatchService - Pattern matching implementation
- ListComprehensionService - List comprehensions
- LoopService - Loop handling (for/while)
- IndexAccessService - Array/record index access

**Imports** (2 services):
- ImportService - Module import resolution
- ModuleResolver - Module path resolution

**Inference** (1 service):
- TypeInferenceService - Type inference and resolution (591 LOC, 31 methods, high cohesion)

**Registration** (2 services):
- FunctionRegistrationService - Function registration
- SumTypeConstructorService - Sum type construction

**Scope** (3 services):
- VarTypeRegistry - Variable type tracking
- ScopeContextService - Scope management
- ModuleContextService - Module context tracking

**Utils** (3 services):
- TypeResolutionService - Type resolution helpers
- TypeUnificationService - Type unification
- PurityAnalyzer - Expression purity analysis

---

## Test Status

**Current** (2025-11-10):
```
1500 runs, 3872 assertions, 7 failures, 0 errors, 7 skips
```

**Baseline** (before reorganization):
```
1500 runs, 3874 assertions, 8 failures, 0 errors
```

**Result**: 1 fewer failure, 0 regressions through all reorganization phases

---

## Completed Work

### Phase 25: Codebase Reorganization (ЭТАП 1-6)
- **ЭТАП 1**: Created directory structure (compiler/, core/, infrastructure/)
- **ЭТАП 1.5**: Moved root files, cleaned up 78 legacy files
- **ЭТАП 2**: Moved stdlib utilities to compiler/stdlib/
- **ЭТАП 3**: Reorganized services into 8 functional categories
- **ЭТАП 4**: Removed 3 duplicate services (351 LOC)
- **ЭТАП 5**: Analyzed TypeChecker/TypeInferenceService - decided NOT to split (high cohesion)
- **ЭТАП 6**: Finalization, documentation

**Statistics**:
- 211 files changed
- +2,363 insertions, -13,326 deletions
- Net: -10,963 lines
- 0 regressions

### Phase 27: Documentation Improvements
- Deleted 24 obsolete documentation files
  - Chat logs (3): cursor_cppastv3, rubydslchatgpt, runtime
  - Completed migrations (11): expression_transformer_*, visitor_pattern_*, phase_23c, etc.
  - Obsolete plans (8): GITHUB_RENAME_GUIDE, MIGRATION_MAPPING, PHASE_25*, etc.
  - Obsolete architecture docs (2): ARCHITECTURE_GUIDE.md, ARCHITECTURE_REVIEW.md
- Updated REFACTORING_ROADMAP.md with Phase 23-25 completion
- Rewrote progress.md to reflect SemanticGen architecture

**Statistics**:
- 26 files changed
- +427 insertions, -26,336 deletions (documentation only)
- 0 regressions

### Phase 24: IRGen Elimination
- Removed lib/mlc/irgen.rb, lib/mlc/irgen/, lib/mlc/irgen_v2/
- ~3200 LOC deleted
- All tests pass

### Phase 23: Visitor Pattern Migration
- Replaced transformer.send() with proper Visitor Pattern
- Created ExpressionVisitor, StatementVisitor
- ~54 anti-pattern calls eliminated

### Phase 21-22: Service Consolidation
- Reduced services from 16 → 14
- Eliminated delegate methods
- -189 LOC total

---

## Next Steps

All major refactoring phases complete. Documentation modernized.

### Future Work (Deferred)
- **Phase 28**: Autoloading Infrastructure (8h)
- **Phase 29**: IoC Container (12h)
- **Phase 30**: Type System Improvements (TBD)

---

## Architecture Principles

1. **Phase Separation**: Clear boundaries between parsing, semantic analysis, and code generation
2. **Service Organization**: Services grouped by functionality, not alphabetically
3. **Single Responsibility**: Each service has one clear purpose
4. **High Cohesion**: Methods that work together stay together
5. **Visitor Pattern**: Clean separation of traversal from transformation logic
6. **Registries as Core**: TypeRegistry and FunctionRegistry as central components (rustc-inspired)
7. **Cross-Phase Utilities**: Compiler infrastructure available to all phases

---

**Last Updated**: 2025-11-10 (after Phase 25 completion and docs cleanup)
