# Phase 28 Analysis: Zeitwerk Autoloading Feasibility

**Date**: 2025-11-15
**Status**: Deferred - Blockers Found

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

## Recommended Next Steps

1. ✅ Document findings in REFACTORING_ROADMAP.md (DONE)
2. Implement Phase 27.5 before attempting Phase 28
3. After Phase 27.5 completion, re-analyze for any additional blockers
4. Implement Phase 28 only after all namespace mismatches are resolved

## Notes

- Current manual require system works but requires maintenance
- Zeitwerk would eliminate ~293 manual require statements
- Benefits are significant but not critical for current development
- Deferring is appropriate given structural prerequisites needed

## References

- REFACTORING_ROADMAP.md Phase 28 (updated with analysis)
- REFACTORING_ROADMAP.md Phase 25 (defines target architecture)
- Zeitwerk documentation: https://github.com/fxn/zeitwerk
