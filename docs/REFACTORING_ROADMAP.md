# MLC Compiler Refactoring Roadmap

## Overview
Comprehensive architecture refactoring to eliminate code complexity, improve maintainability, and follow Ruby best practices.

## Completed Phases

### ✅ Phase 21: Service Consolidation (6h)
**Status**: COMPLETE
**Results**: Services reduced from 16 → 14 (-12%, -141 LOC)

- Deleted GenericCallResolverService (63 lines) - thin wrapper
- Deleted LambdaTypeInferenceService (78 lines) - duplicate functionality
- Moved stdlib files to services/ for better organization
- Fixed delegate method bugs
- All tests pass ✅

### ✅ Phase 22: Eliminate Delegate Methods (3h)
**Status**: COMPLETE
**Results**: -48 LOC (removed 8 delegate methods)

- Removed 8 delegate methods from IRGen (~50 lines)
- Updated 1 call site to use direct service access
- Fixed test to use function_registry.lookup()
- Fixed Phase 21 leftover require path issues
- All tests pass ✅

## Current Architecture State

**Services** (14):
1. VarTypeRegistry - Variable type tracking
2. TypeChecker - Type validation and compatibility
3. TypeInferenceService - Type inference and resolution
4. RecordBuilderService - Record construction
5. PurityAnalyzer - Expression purity analysis
6. ModuleContextService - Module context management
7. TypeResolutionService - Type resolution
8. FunctionRegistrationService - Function registration
9. SumTypeConstructorService - Sum type construction
10. TypeUnificationService - Type unification
11. FunctionLookupService - Function lookup
12. RuleEngineBuilder - Rule engine configuration
13. StdlibResolver - Stdlib module resolution
14. StdlibSignatureRegistry - Stdlib signature tracking

## Pending Phases

### 🔴 Phase 23: Eliminate transformer.send() Anti-pattern (16h, CRITICAL)
**Goal**: Remove architectural coupling between Rules and Transformer internals

**Current Problem**:
- **54 calls** to `transformer.send(:method_name)` in rules/
- **1 call** in services/record_builder_service.rb
- Rules directly access private transformer methods via `send()`
- Violates separation of concerns - rules should only use services

**Breakdown**:
```ruby
# Most common violations:
transformer.send(:transform_expression, ...)  # ~35 calls
transformer.send(:transform_type, ...)        # ~8 calls
transformer.send(:transform_block_expr, ...)  # ~4 calls
transformer.send(:transform_statement_block, ...) # ~3 calls
transformer.send(:unit_branch_ast?, ...)      # ~2 calls
transformer.send(:module_member_function_entry, ...) # ~1 call
```

**Solution Options**:

**Option A: ExpressionTransformationService** (Recommended)
```ruby
# Create new service to encapsulate transformation logic
class ExpressionTransformationService
  def transform_expression(node) ... end
  def transform_type(node) ... end
  def transform_block_expr(node) ... end
  def transform_statement_block(node) ... end
end

# Rules access via service, not transformer
context = {
  expr_transformer: expr_transformation_service,
  type_transformer: type_transformation_service,
  ...
}
```

**Option B: Visitor Pattern** (More invasive)
```ruby
# Each rule becomes a Visitor
class BinaryRule < BaseRule
  def visit_binary(node, context)
    left = accept(node.left, context)
    right = accept(node.right, context)
    ...
  end
end
```

**Tasks**:
1. **Analysis Phase** (2h):
   - Catalog all 54 `transformer.send()` call sites
   - Group by method name and usage pattern
   - Determine which methods belong in services vs. utilities

2. **Service Design** (3h):
   - Create `ExpressionTransformationService`
   - Create `TypeTransformationService`
   - Create `StatementTransformationService`
   - Define clean public APIs

3. **Migration** (8h):
   - Update all 54 call sites in rules
   - Update 1 call site in record_builder_service
   - Add services to rule context hashes
   - Remove private method access

4. **Cleanup** (2h):
   - Make transformer methods properly private
   - Update tests
   - Verify no remaining `send()` anti-patterns

5. **Verification** (1h):
   - Run full test suite
   - Check for regressions

**Benefits**:
- Proper separation of concerns
- Rules depend on stable service interfaces
- Transformer internals can change without breaking rules
- Easier to test rules in isolation
- No more reflection/meta-programming hacks

---

### Phase 24: Rename lib/mlc/irgen → lib/mlc/transformers (2h, Low)
**Goal**: More descriptive directory name

**Tasks**:
1. Move `lib/mlc/irgen` → `lib/mlc/transformers`
2. Move `lib/mlc/irgen.rb` → `lib/mlc/transformers.rb`
3. Rename class `MLC::IRGen` → `MLC::Transformers::IRGen`
4. Update all require_relative statements
5. Update tests
6. Update documentation

**Benefits**:
- Clearer semantics (`transformers` vs `irgen`)
- Better project organization
- Matches Rails conventions

---

### Phase 25: Type System Directory Review (2h, Medium)
**Goal**: Review type_system/ directory - should it be in services/?

**Current State**:
```
lib/mlc/type_system/
  ├── effect_analyzer.rb (817 bytes)
  ├── generic_call_resolver.rb (317 bytes)
  ├── match_analyzer.rb (3248 bytes)
  └── type_constraint_solver.rb (1293 bytes)
```

**Questions**:
1. Are these "services" or "algorithmic components"?
2. Should they move to `services/type_system/`?
3. Or keep separate as "core type system algorithms"?

**Analysis Needed**:
- Review responsibilities of each component
- Check if they manage state (services) or pure algorithms
- Decide on architectural separation
- Document the decision

**Options**:
A. Move to `services/` (unified service architecture)
B. Keep in `type_system/` (separate algorithmic layer)
C. Split: some to services/, core algorithms stay

**Benefits**:
- Consistent architecture
- Clear separation of concerns
- Better documentation

---

### Phase 26: Root Directory Review (2h, Medium)
**Goal**: Review lib/mlc root files - which should move to subdirectories?

**Current State**:
```
lib/mlc/
  ├── application.rb (1.3K) - Entry point
  ├── event_bus.rb (5.0K) - Infrastructure
  ├── function_registry.rb (11K) - Registry
  ├── irgen.rb (11K) - Main transformation orchestrator
  ├── pass_manager.rb (5.5K) - Compilation pipeline
  └── type_registry.rb (9.7K) - Registry
```

**Questions**:
1. Should registries move to `lib/mlc/registries/`?
   - `function_registry.rb`
   - `type_registry.rb`
2. Should infrastructure components move to `lib/mlc/infrastructure/`?
   - `event_bus.rb`
   - `pass_manager.rb`
3. Should `application.rb` and `irgen.rb` stay in root (they're top-level)?

**Analysis Needed**:
- Review directory structure consistency
- Check if subdirectories improve organization
- Consider impact on require paths
- Document the decision

**Options**:
A. Move registries to `registries/` subdirectory
B. Move infrastructure to `infrastructure/` subdirectory
C. Keep all in root (minimal nesting, clear top-level API)
D. Hybrid: move some, keep main orchestrators in root

**Benefits**:
- Clearer separation by responsibility
- Easier to locate files by category
- More scalable organization

---

### Phase 27: Phase Comment Cleanup (1h, Low)
**Goal**: Remove all "Phase XX" comments from codebase

**Tasks**:
1. Search for "Phase" comments in all .rb files
2. Remove or replace with descriptive comments
3. Update documentation

**Benefits**:
- Cleaner codebase
- Better code readability

---

### Phase 28: Autoloading Infrastructure (8h, High)
**Goal**: Implement Rails-style autoloading for rules, services, and type system

**Current Problem**:
- 50+ manual `require_relative` statements in irgen.rb
- Every new rule/service requires manual require
- Error-prone and tedious maintenance

**Solution**: Zeitwerk-style autoloading

**Implementation**:
```ruby
# lib/mlc/autoloader.rb
module MLC
  class Autoloader
    def self.setup
      loader = Zeitwerk::Loader.new
      loader.push_dir(File.expand_path('../', __dir__))

      # Configure inflections
      loader.inflector.inflect(
        "irgen" => "IRGen",
        "high_ir" => "HighIR",
        "ast" => "AST"
      )

      # Eager load critical paths
      loader.eager_load_dir("#{__dir__}/rules")
      loader.eager_load_dir("#{__dir__}/services")
      loader.eager_load_dir("#{__dir__}/type_system")

      loader.setup
      loader.eager_load
    end
  end
end
```

**Directory Structure**:
```
lib/mlc/
  ├── rules/
  │   ├── irgen/
  │   │   ├── expression/    # Auto-loads all *_rule.rb
  │   │   └── statement/     # Auto-loads all *_rule.rb
  │   └── rule_engine.rb
  ├── services/              # Auto-loads all *_service.rb
  ├── type_system/           # Auto-loads TypeConstraintSolver, etc.
  └── irgen.rb               # No more require_relative!
```

**Benefits**:
- Eliminate 50+ require_relative statements
- Automatic loading of new rules/services
- Faster development iteration
- Standard Ruby convention

**Effort**: 8h
- Setup Zeitwerk (2h)
- Configure inflections (1h)
- Test autoloading (2h)
- Update documentation (1h)
- Fix edge cases (2h)

---

### Phase 29: Ruby IoC Container (12h, High)
**Goal**: Replace manual dependency injection with proper IoC container

**Current Problem**:
- IRGenContainer builds all services manually (200+ lines)
- Hash-based context in rules (tight coupling)
- Difficult to test individual components
- No lifecycle management

**Solution**: Dry-container + Dry-auto_inject

**Implementation**:
```ruby
# lib/mlc/container.rb
require 'dry-container'
require 'dry-auto_inject'

module MLC
  class Container
    extend Dry::Container::Mixin

    # Register services with lifecycle
    register(:event_bus, memoize: true) { EventBus.new }
    register(:type_registry, memoize: true) { TypeRegistry.new }
    register(:function_registry, memoize: true) { FunctionRegistry.new }

    # Inject dependencies automatically
    register(:type_checker) do
      Services::TypeChecker.new(
        function_registry: resolve(:function_registry),
        type_decl_table: resolve(:type_decl_table),
        event_bus: resolve(:event_bus)
      )
    end

    # Factory for IRGen (new instance each time)
    register(:irgen, memoize: false) do
      IRGen.new(
        rule_engine: resolve(:rule_engine),
        type_registry: resolve(:type_registry),
        # ... auto-inject all dependencies
      )
    end
  end

  # Auto-injection module
  Import = Dry::AutoInject(Container)
end

# Usage in services:
module MLC
  module Services
    class TypeChecker
      include Import[:function_registry, :event_bus, :type_decl_table]

      # Dependencies injected automatically!
      def initialize(**deps)
        super
      end
    end
  end
end

# Usage in rules:
module MLC
  module Rules
    module IRGen
      module Expression
        class LambdaRule < BaseRule
          include Import[:transformer, :type_checker, :scope_context]

          def apply(node, context = {})
            # Use @transformer, @type_checker, @scope_context directly
            # No more context hash lookup!
          end
        end
      end
    end
  end
end
```

**Benefits**:
- Explicit dependencies
- Easy to test (swap dependencies)
- Lifecycle management
- Standard Ruby practice
- Eliminates hash-based context coupling

**Migration Path**:
1. Add dry-container gem
2. Create Container
3. Register core services (TypeRegistry, FunctionRegistry)
4. Register dependent services with auto-injection
5. Update IRGen to use container
6. Update rules to use Import[] injection
7. Delete IRGenContainer
8. Delete context hash building code

**Effort**: 12h
- Setup container (2h)
- Migrate services (4h)
- Migrate rules (3h)
- Update tests (2h)
- Documentation (1h)

---

### Phase 30: Service Layer Documentation (4h, Medium)
**Goal**: Document each service's responsibilities and dependencies

**Tasks**:
1. Create ARCHITECTURE_GUIDE.md
2. Document service layers
3. Create dependency diagram
4. Document IoC container usage

---

### Phase 31: Type System Improvements (18h, High)
**Goal**: Clean up type system architecture

*Detailed plan to be created after Phase 22-26 complete*

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

**Immediate Priority** (Next 52h):
1. ✅ Phase 21: Service Consolidation (DONE)
2. ✅ Phase 22: Eliminate Delegate Methods (DONE)
3. 🔴 **Phase 23: Eliminate transformer.send() Anti-pattern (16h)** ← CRITICAL
4. Phase 24: Rename irgen → transformers (2h)
5. Phase 25: Type System Directory Review (2h)
6. Phase 26: Root Directory Review (2h)
7. Phase 27: Phase Comment Cleanup (1h)
8. Phase 28: Autoloading Infrastructure (8h)
9. Phase 29: Ruby IoC Container (12h)
10. Phase 30: Documentation (4h)
11. Phase 31: Type System Improvements (18h+, deferred)

**Critical Path**: Phase 23 must be completed before Phase 29 (IoC Container)

**Total Estimated Effort**: 52 hours (excluding Phase 31)
**Expected Benefits**:
- Eliminate 54+ transformer.send() anti-patterns
- Proper separation of concerns (Rules → Services, not Transformer)
- Eliminate 200+ lines of boilerplate
- Remove 50+ require_relative statements
- Standard Ruby architecture patterns
- Easier testing and maintenance
- Foundation for future improvements
