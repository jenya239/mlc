# Phase 28 Analysis: Zeitwerk Autoloading Feasibility

**Date**: 2025-11-16 (Updated)
**Status**: In Progress - Low-Priority Blockers Resolved

## Analysis Summary

Phase 28 (Zeitwerk Autoloading Infrastructure) was analyzed for implementation feasibility. The analysis revealed critical blockers that prevent immediate implementation.

## Findings

### Current State
- **Total Ruby files**: 154 files in lib/mlc/
- **Manual requires**: 293 require_relative statements across codebase
- **Entry point**: lib/mlc/common/index.rb (31 requires)
- **Main loaders**:
  - lib/mlc/representations/semantic/semantic_gen.rb (49 requires)
  - lib/mlc/backends/cpp.rb
  - lib/mlc/common/index.rb

### Critical Blockers

#### 1. Directory/Namespace Mismatches

**Problem**: Zeitwerk requires strict file path → namespace correspondence.

**Found mismatches**:
- `lib/mlc/common/registry/function_registry.rb` defines `MLC::Core::FunctionRegistry`
  - **Expected by Zeitwerk**: `MLC::Common::Registry::FunctionRegistry`
  - **Actual**: `MLC::Core::FunctionRegistry`

- `lib/mlc/common/registry/function_signature.rb` defines `MLC::Core::FunctionSignature`
  - **Expected by Zeitwerk**: `MLC::Common::Registry::FunctionSignature`
  - **Actual**: `MLC::Core::FunctionSignature`

- `lib/mlc/common/registry/type_registry.rb` defines `MLC::Core::TypeRegistry`
  - **Expected by Zeitwerk**: `MLC::Common::Registry::TypeRegistry`
  - **Actual**: `MLC::Core::TypeRegistry`

**Impact**: 8 references to `MLC::Core::` across 6 files:
1. lib/mlc/representations/semantic/gen/engine.rb (2 refs)
2. lib/mlc/representations/semantic/gen/services/utils/type_unification_service.rb (1 ref)
3. lib/mlc/representations/semantic/gen/services/registration/sum_type_constructor_service.rb (1 ref)
4. lib/mlc/representations/semantic/gen/services/registration/function_registration_service.rb (1 ref)
5. lib/mlc/representations/semantic/gen/services/imports/import_service.rb (1 ref)
6. lib/mlc/representations/semantic/gen/pipeline.rb (2 refs)

#### 2. Missing Directory Structure

**Problem**: `lib/mlc/core/` directory does not exist, but classes use `MLC::Core::` namespace.

**Actual structure**:
```
lib/mlc/
├── backends/
├── common/
│   └── registry/        # ← Files here define MLC::Core::*
├── representations/
└── source/
```

**Expected for Zeitwerk (according to REFACTORING_ROADMAP.md Phase 25)**:
```
lib/mlc/
├── core/                # ← Should exist for MLC::Core::*
├── compiler/
├── semantic_gen/
├── analysis/
├── backend/
└── ...
```

## Prerequisites for Phase 28

### Phase 27.5: Fix Directory/Namespace Mismatches (2h estimated)

**Tasks**:
1. Create `lib/mlc/core/` directory
2. Move 3 registry files:
   - `lib/mlc/common/registry/function_registry.rb` → `lib/mlc/core/function_registry.rb`
   - `lib/mlc/common/registry/function_signature.rb` → `lib/mlc/core/function_signature.rb`
   - `lib/mlc/common/registry/type_registry.rb` → `lib/mlc/core/type_registry.rb`
3. Update require paths in 6 files (8 total references)
4. Update loader files (lib/mlc/common/index.rb, etc.)
5. Run full test suite to verify no regressions

**Files to update** (require path changes):
- lib/mlc/representations/semantic/gen/engine.rb
- lib/mlc/representations/semantic/gen/services/utils/type_unification_service.rb
- lib/mlc/representations/semantic/gen/services/registration/sum_type_constructor_service.rb
- lib/mlc/representations/semantic/gen/services/registration/function_registration_service.rb
- lib/mlc/representations/semantic/gen/services/imports/import_service.rb
- lib/mlc/representations/semantic/gen/pipeline.rb

**Risk**: Low - only 3 files to move, 8 references to update

## Progress Update (2025-11-16)

### ✅ Phase 27.5 Completed

**Commit 6d2a7ce** (2025-11-15):
- Moved 3 registry files from `lib/mlc/common/registry/` to `lib/mlc/registries/`
- Changed namespace: `MLC::Core::` → `MLC::Registries::`
- Updated 22 files with new references
- **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

**Files moved**:
- `function_registry.rb` → `lib/mlc/registries/function_registry.rb`
- `function_signature.rb` → `lib/mlc/registries/function_signature.rb`
- `type_registry.rb` → `lib/mlc/registries/type_registry.rb`

### ✅ Additional Namespace Fixes (2025-11-16)

**Comprehensive Analysis**:
- Scanned entire codebase for namespace mismatches
- Found 13 categories of mismatches
- Prioritized by usage count and impact

**Low-Priority Fixes Completed**:

1. **Commit 28bd2b4**: `MLC::Compiler` → `MLC::Common::Stdlib`
   - Fixed: `lib/mlc/common/stdlib/scanner.rb`, `signature_registry.rb`

2. **Commit 4adf7ff**: `MLC::Compiler` → `MLC::Common::Stdlib`
   - Fixed: `lib/mlc/common/stdlib/resolver.rb`
   - Updated: 12 test file references

3. **Commit 5212b50**: Multiple namespace fixes
   - `MLC::Compiler::MetadataLoader` → `MLC::Common::Stdlib::MetadataLoaderService`
   - `MLC::Stdlib` → `MLC::Common::Stdlib`
   - Files: `metadata_loader.rb`, `registry.rb`, `container.rb`

4. **Commit a394d7b**: `MLC::Backend` → `MLC::Backends::Cpp`
   - Fixed 3 definition files: `header_generator.rb`, `metadata_generator.rb`, `runtime_policy.rb`
   - Updated 9 usage files (12 files total, 39 references)
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

5. **Commit 56c886c**: `MLC::TypeSystem` → `MLC::Common::Typing`
   - Fixed 4 definition files in `lib/mlc/common/typing/`:
     * `effect_analyzer.rb`, `generic_call_resolver.rb`, `match_analyzer.rb`, `type_constraint_solver.rb`
   - Updated 19 references across 6 files
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

6. **Commit b8018a9**: `StdlibSignatureRegistry` → `SignatureRegistry`
   - Fixed: `lib/mlc/common/stdlib/signature_registry.rb`
   - Избыточный префикс "Stdlib" удалён
   - Updated 2 references (container.rb, test file)
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

7. **Commit f69baf6**: `MetadataLoaderService` → `MetadataLoader`
   - Fixed: `lib/mlc/common/stdlib/metadata_loader.rb`
   - Избыточный суффикс "Service" удалён
   - Updated 2 references (container.rb, test file)
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

8. **Commit a68f849**: `StdlibResolver` → `Resolver`
   - Fixed: `lib/mlc/common/stdlib/resolver.rb`
   - Избыточный префикс "Stdlib" удалён
   - Updated 12 references in 2 test files
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

9. **Commit 3db8f2f**: `StdlibScanner` → `Scanner`
   - Fixed: `lib/mlc/common/stdlib/scanner.rb`
   - Избыточный префикс "Stdlib" удалён
   - Updated 26 references across 6 files
   - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

10. **Commit dd6aef1**: `CodeGen` → `Codegen` (CamelCase fix)
    - Fixed: `lib/mlc/backends/cpp/codegen.rb`
    - Zeitwerk требует lowercase для filename → class name match
    - Updated 11 references across 5 files
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

11. **Commit be4e799**: `MLC::Analysis` → `MLC::Common::Analysis`
    - Fixed 4 files in `lib/mlc/common/analysis/`:
      * `base_pass.rb`, `effect_analysis_pass.rb`, `name_resolution_pass.rb`, `type_check_pass.rb`
    - Updated 7 references in 2 test files
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

12. **Commit 2c8daf8**: `MLC::Infrastructure` and `MLC::Diagnostics` namespace fixes (2025-11-16)
    - **EventBus**: `MLC::Infrastructure::EventBus` → `MLC::Common::Diagnostics::EventBus`
    - **PassManager**: `MLC::Infrastructure::PassManager` → `MLC::Common::Analysis::PassManager`
    - **Diagnostics**: All files in `lib/mlc/common/diagnostics/` updated to use `MLC::Common::Diagnostics`
    - Files modified: 13 total (7 production + 6 test)
      * lib/mlc/common/diagnostics/event_bus.rb
      * lib/mlc/common/diagnostics/error_formatter.rb
      * lib/mlc/common/diagnostics/severity.rb
      * lib/mlc/common/diagnostics/issue.rb
      * lib/mlc/common/diagnostics/diagnostic_store.rb
      * lib/mlc/common/analysis/pass_manager.rb
      * lib/mlc/common/index.rb
      * 6 test files updated
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

13. **Commit b777ff8 + 892fbfc**: `MLC::AST` → `MLC::Source::AST` (2025-11-17)
    - Fixed: All files in `lib/mlc/source/ast/`
    - Wrapped `module AST` in `module Source` in nodes.rb
    - Fixed duplicated namespaces from sed automation: `MLC::Source::MLC::Source::AST::` → `MLC::Source::AST::`
    - Fixed bare `AST::` references in:
      * lib/mlc/source/parser/base_parser.rb:90
      * lib/mlc/source/parser/optimized_parser.rb (3 locations)
      * lib/mlc/representations/semantic/gen/services/checkers/type_checker.rb (3 locations)
      * lib/mlc/common/stdlib/scanner.rb (5 locations)
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

14. **Commit 0f7693c**: `MLC::Parser` → `MLC::Source::Parser` (2025-11-17)
    - Fixed: All 10 parser files in `lib/mlc/source/parser/`
    - Wrapped all `module Parser` in `module Source`:
      * parser.rb, base_parser.rb, declaration_parser.rb
      * expression_parser.rb, statement_parser.rb, type_parser.rb
      * pattern_parser.rb, error_recovery_parser.rb
      * lexer.rb, optimized_parser.rb
    - Replaced all `MLC::Parser::` → `MLC::Source::Parser::`
    - Fixed bare `Parser::Parser` references in:
      * lib/mlc/common/compiler_api.rb
      * lib/mlc/common/stdlib/scanner.rb
    - Updated 4 test files with new namespace
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅

15. **Commit f9843d1**: `MLC::Rules` → `MLC::Representations::Semantic::Gen::Services` (2025-11-17)
    - Fixed: lib/mlc/representations/semantic/gen/services/rule_engine.rb
    - Wrapped RuleEngine in proper hierarchy:
      * Changed: module Rules → module Representations::Semantic::Gen::Services
    - Updated reference in lib/mlc/representations/semantic/gen/engine.rb
    - Updated 3 references in test/mlc/rules_rule_engine_test.rb
    - **Test results**: 1524 runs, 4014 assertions, 0 failures, 0 errors ✅
    - **Milestone**: All 5 high-priority namespace migrations complete!

### ✅ High-Priority Namespace Migrations - **COMPLETE!**

Based on comprehensive codebase scan (updated 2025-11-17):

**All High-Impact Categories - COMPLETED** (2025-11-17):

1. ✅ **MLC::AST** (256 usages) → **COMPLETED** (commits b777ff8, 892fbfc)
   - Migrated to `MLC::Source::AST::*`

2. ✅ **MLC::SemanticGen** (54+ usages) → **COMPLETED**
   - Migrated to `MLC::Representations::Semantic::Gen::*`

3. ✅ **MLC::Services** (42 usages) → **COMPLETED**
   - Migrated to `MLC::Representations::Semantic::Gen::Services::*`

4. ✅ **MLC::Parser** (126 usages) → **COMPLETED** (commit 0f7693c)
   - Migrated to `MLC::Source::Parser::*`

5. ✅ **MLC::Rules** (4 usages) → **COMPLETED** (commit f9843d1)
   - Migrated to `MLC::Representations::Semantic::Gen::Services::*`
   - Note: lib/mlc/backends/cpp/rules/* already had correct namespace MLC::Backends::Cpp::Rules

**All Medium-Impact Categories - COMPLETED** (2025-11-16):

6. ✅ **MLC::Infrastructure** (80 usages) → **COMPLETED** (commit 2c8daf8)
   - EventBus: `MLC::Infrastructure::EventBus` → `MLC::Common::Diagnostics::EventBus`
   - PassManager: `MLC::Infrastructure::PassManager` → `MLC::Common::Analysis::PassManager`

7. ✅ **MLC::Diagnostics** (40 usages) → **COMPLETED** (commit 2c8daf8)
   - Migrated to `MLC::Common::Diagnostics::*`

**Impact Analysis**:
- **Total high-priority usages**: ~560+
- **Estimated effort**: 3-5 days for systematic refactoring
- **Risk**: High - requires coordinated changes across entire codebase
- **Recommendation**: Defer until critical development milestones completed

## Recommended Next Steps

1. ✅ Document findings in REFACTORING_ROADMAP.md (DONE)
2. ✅ Implement Phase 27.5 (DONE - commit 6d2a7ce)
3. ✅ Fix low-priority namespace mismatches (DONE - 11 commits total):
   - 28bd2b4, 4adf7ff, 5212b50, a394d7b (previous session - 4 fixes)
   - 56c886c, b8018a9, f69baf6, a68f849, 3db8f2f, dd6aef1, be4e799 (2025-11-16 - 7 fixes)
4. ✅ Fix medium-priority namespace mismatches (DONE - commit 2c8daf8, 2025-11-16):
   - MLC::Infrastructure → MLC::Common::Diagnostics / MLC::Common::Analysis
   - MLC::Diagnostics → MLC::Common::Diagnostics
5. ✅ Fix high-priority namespace mismatches (DONE - 4 commits, 2025-11-17):
   - b777ff8, 892fbfc (MLC::AST → MLC::Source::AST)
   - 0f7693c (MLC::Parser → MLC::Source::Parser)
   - f9843d1 (MLC::Rules → MLC::Representations::Semantic::Gen::Services)
6. ✅ **Phase 27.5 Complete!** All namespace migrations finished (16 commits total)
7. ⏸️ Phase 28 (Zeitwerk) implementation - ready to proceed when needed

## Notes

- Current manual require system works but requires maintenance
- Zeitwerk would eliminate ~293 manual require statements
- Benefits are significant but not critical for current development
- Deferring is appropriate given structural prerequisites needed

## References

- REFACTORING_ROADMAP.md Phase 28 (updated with analysis)
- REFACTORING_ROADMAP.md Phase 25 (defines target architecture)
- Zeitwerk documentation: https://github.com/fxn/zeitwerk
