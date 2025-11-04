# Visitor Pattern Migration Plan

## Current Architecture Status (Phase 23 - Hybrid)

### What We Have Now

**HYBRID ARCHITECTURE**: IRGen uses both transformer mixins AND visitor classes

```ruby
class IRGen
  # Transformer mixins (included into IRGen)
  include ExpressionTransformer
  include StatementTransformer
  include PatternMatchingTransformer
  include TypeTransformer
  include FunctionDeclarationTransformer
  include StdlibImportTransformer
  include ProgramTransformer

  # Visitor instances (separate objects)
  @expression_visitor  # Visitors::ExpressionVisitor
  @statement_visitor   # Visitors::StatementVisitor
end
```

### Problems with Current Hybrid Approach

1. **Duplication**: Both transformer mixins and visitors exist for expressions/statements
2. **Confusion**: Unclear which code path is used (mixin vs visitor)
3. **Testing complexity**: Need to test both paths
4. **Maintenance burden**: Changes must be made in two places
5. **Circular dependencies**: Visitors need transformer reference, transformers need visitor instances

## Transformer Modules Analysis

### Expression/Statement (Phase 23 - Partially Migrated)

**Status**: ✅ Visitor classes exist, ⚠️ Transformer mixins still used

- `lib/mlc/irgen/expression_transformer.rb` → `lib/mlc/visitors/expression_visitor.rb`
- `lib/mlc/irgen/statement_transformer.rb` → `lib/mlc/visitors/statement_visitor.rb`

**Visitors created** in Phase 23-A and 23-C, but transformer mixins not removed yet.

### Pattern Matching (Not migrated)

**Status**: ❌ Only transformer mixin exists

- `lib/mlc/irgen/pattern_matching_transformer.rb`

Pattern matching is only used by match expressions. Could be:
- Integrated into ExpressionVisitor
- Kept as separate PatternMatchingVisitor
- **Recommendation**: Keep as mixin (used in only one place)

### Type Transformer (Not migrated)

**Status**: ❌ Only transformer mixin exists

- `lib/mlc/irgen/type_transformer.rb`

Transforms type declarations. Not really visitor pattern candidate - it's a one-shot transformation, not traversal.

**Recommendation**: Keep as mixin or extract to service class

### Function Declaration Transformer (Not migrated)

**Status**: ❌ Only transformer mixin exists

- `lib/mlc/irgen/function_declaration_transformer.rb`

Similar to TypeTransformer - transforms function declarations, not traversal.

**Recommendation**: Keep as mixin or extract to service class

### Stdlib Import Transformer (Not migrated)

**Status**: ❌ Only transformer mixin exists

- `lib/mlc/irgen/stdlib_import_transformer.rb`

Handles stdlib imports. Service-like functionality, not traversal.

**Recommendation**: Extract to StdlibImportService

### Program Transformer (Not migrated)

**Status**: ❌ Only transformer mixin exists

- `lib/mlc/irgen/program_transformer.rb`

Orchestrates full program transformation via PassManager.

**Recommendation**: Keep as mixin (orchestration layer)

## Migration Plan

### Phase 25-A: Complete Expression/Statement Migration

**Goal**: Remove ExpressionTransformer and StatementTransformer mixins

1. Find all calls to mixin methods
2. Replace with visitor method calls
3. Remove `include ExpressionTransformer` and `include StatementTransformer`
4. Delete mixin files
5. Update tests

**Risk**: LOW - Visitors already exist and are tested

### Phase 25-B: Refactor Non-Visitor Transformers

**Goal**: Extract service-like transformers to proper service classes

1. Extract StdlibImportTransformer → StdlibImportService (already exists!)
2. Keep TypeTransformer as mixin (one-shot, not traversal)
3. Keep FunctionDeclarationTransformer as mixin (one-shot)
4. Keep PatternMatchingTransformer as mixin (single use case)
5. Keep ProgramTransformer as mixin (orchestration)

**Risk**: LOW - Already follows service architecture

### Phase 25-C (Optional): Full Visitor Pattern

**Goal**: If desired, create visitors for ALL AST traversal

1. Create TypeDeclVisitor
2. Create FunctionDeclVisitor
3. Create PatternVisitor
4. Remove ALL transformer mixins from IRGen
5. IRGen becomes pure orchestration + services

**Risk**: MEDIUM - Significant refactoring, questionable benefit

## Recommendation

### ✅ DO THIS (Phase 25-A)

Complete the expression/statement migration:
- Remove ExpressionTransformer mixin
- Remove StatementTransformer mixin
- Use only ExpressionVisitor and StatementVisitor

### ⚠️ CONSIDER (Phase 25-B)

Keep remaining transformers as mixins - they are NOT visitor candidates:
- TypeTransformer - one-shot transformation
- FunctionDeclarationTransformer - one-shot transformation
- ProgramTransformer - orchestration layer
- PatternMatchingTransformer - single use case

These are better as mixins or services, NOT visitors.

### ❌ DON'T DO (Phase 25-C)

Full visitor pattern migration is **overkill** for this compiler:
- Not all AST operations benefit from visitor pattern
- Current hybrid is fine for non-recursive transformations
- Engineering time better spent on features

## Final Architecture (After Phase 25-A+B)

```ruby
class IRGen
  # Visitors (for recursive AST traversal)
  @expression_visitor  # Handles all expression nodes
  @statement_visitor   # Handles all statement nodes

  # Transformer Mixins (for one-shot transformations)
  include TypeTransformer              # transform_type_decl
  include FunctionDeclarationTransformer  # transform_function
  include PatternMatchingTransformer    # transform_pattern
  include ProgramTransformer            # transform_program (orchestration)

  # Services (injected dependencies)
  @stdlib_import_service
  @type_resolution_service
  @function_registration_service
  # ... other services
end
```

This is a **pragmatic hybrid** that uses visitor pattern where it makes sense (recursive traversal) and keeps simpler patterns for other use cases.

## Multi-File Modules (Separate Issue)

The 6 skipped tests in `multi_file_modules_test.rb` require:

1. **CLI enhancement**: Support compiling multiple .mlc files
2. **Metadata generation**: Generate .mlcmeta files during compilation
3. **Import resolution**: Load dependencies before compiling

This is **NOT** related to visitor pattern - it's a separate feature requiring:
- Changes to `bin/mlc` CLI
- New MetadataWriter service (opposite of MetadataLoader)
- Multi-file compilation orchestration

**Estimated effort**: 2-3 days of work

**Status**: Phase 24-B (MetadataLoader) is complete, but CLI changes needed

## Summary

**Current State**: Hybrid (transformers + visitors)
**Recommended Target**: Pragmatic hybrid (visitors for traversal, mixins for one-shot)
**Migration Effort**: Phase 25-A (remove duplicate transformers) - ~4 hours
**Multi-file modules**: Separate feature - ~2-3 days

The visitor pattern is a **tool**, not a **goal**. Use it where it provides value (recursive AST traversal), but don't force it everywhere.
