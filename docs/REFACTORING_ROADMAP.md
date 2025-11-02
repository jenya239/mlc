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

### Phase 22: Eliminate Delegate Methods (3h, Medium)
**Goal**: Remove unnecessary abstraction layers

Current delegate methods in IRGen:
```ruby
# Type unification (5 methods)
unify_type → @type_unification_service
type_equivalent? → @type_unification_service
constructor_info_for → @type_unification_service
generic_substitutions → @type_unification_service
apply_type_substitutions → @type_unification_service

# Function lookup (3 methods)
builtin_function_info → @function_lookup_service
lookup_function_info → @function_lookup_service
function_placeholder_type → @function_lookup_service
```

**Tasks**:
1. Find all call sites of delegate methods
2. Add missing services to rule context hashes
3. Replace delegate calls with direct service calls
4. Remove delegate methods
5. Verify tests pass

**Benefits**:
- Reduces indirection
- Makes dependencies explicit
- Simplifies IRGen interface

---

### Phase 23: Type System Directory Review (2h, Medium)
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

### Phase 24: Root Directory Review (2h, Medium)
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

### Phase 25: Phase Comment Cleanup (1h, Low)
**Goal**: Remove all "Phase XX" comments from codebase

**Tasks**:
1. Search for "Phase" comments in all .rb files
2. Remove or replace with descriptive comments
3. Update documentation

**Benefits**:
- Cleaner codebase
- Better code readability

---

### Phase 26: Autoloading Infrastructure (8h, High)
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

### Phase 27: Ruby IoC Container (12h, High)
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

### Phase 28: Service Layer Documentation (4h, Medium)
**Goal**: Document each service's responsibilities and dependencies

**Tasks**:
1. Create ARCHITECTURE_GUIDE.md
2. Document service layers
3. Create dependency diagram
4. Document IoC container usage

---

### Phase 29: Type System Improvements (18h, High)
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

**Immediate Priority** (Next 32h):
1. ✅ Phase 21: Service Consolidation (DONE)
2. Phase 22: Eliminate Delegate Methods (3h)
3. Phase 23: Type System Directory Review (2h)
4. Phase 24: Root Directory Review (2h)
5. Phase 25: Phase Comment Cleanup (1h)
6. Phase 26: Autoloading Infrastructure (8h)
7. Phase 27: Ruby IoC Container (12h)
8. Phase 28: Documentation (4h)

**Total Estimated Effort**: 32 hours
**Expected Benefits**:
- Eliminate 200+ lines of boilerplate
- Remove 50+ require_relative statements
- Standard Ruby architecture patterns
- Easier testing and maintenance
- Foundation for future improvements
