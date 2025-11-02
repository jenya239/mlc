# Compiler Architecture Comparison & Recommendations

## Executive Summary

Analysis of best-in-class compiler architectures to guide MLC development:

**Best for Study**: Rust rustc (Query System)
**Most Relevant**: Crystal (Ruby-like, typed)
**Best IR Design**: Swift (Multi-level SIL)

**Recommendation**: Synthesize Rust's Query System, Crystal's Binding System, Swift's Multi-level IR, while keeping MLC's Rule System.

---

## 1. Rust Compiler (rustc) ⭐⭐⭐⭐⭐

### Architecture: Query System (Revolutionary)

```
AST → HIR → MIR → LLVM IR
     ↑      ↑      ↑
   Query  Query  Query
   System System System
```

### Key Innovation: Demand-Driven Compilation

The compiler is organized as a **database of queries** that call each other:

```rust
// Every major compilation step is a query
query type_of(def_id: DefId) -> Ty
query optimized_mir(def_id: DefId) -> Body

// Queries call each other, results are cached
fn codegen_fn(def_id: DefId) {
    let ty = tcx.type_of(def_id);        // Query 1 (cached)
    let mir = tcx.optimized_mir(def_id); // Query 2 (cached)
}
```

### Benefits

1. **Incremental Compilation**: Only recompute changed queries
2. **Automatic Memoization**: Results cached on disk
3. **Parallelization**: Queries can run in parallel (2024 update: 1-3% overhead for thread-safety)
4. **Demand-Driven**: Compute only what's needed

### Applicability to MLC

**HIGH** - Query System is language-agnostic, can be implemented in Ruby:

```ruby
# Proposed for MLC Phase 27
class QuerySystem
  def initialize
    @cache = {}  # Memoization
  end

  def define_query(name, &block)
    define_singleton_method(name) do |*args|
      key = [name, args]
      @cache[key] ||= block.call(*args)
    end
  end
end

# Usage
queries = QuerySystem.new
queries.define_query(:type_of) { |node| TypeInferenceService.infer_type(node) }
queries.define_query(:transform_node) { |node| IRGen.transform(node) }

# Automatic caching!
ty = queries.type_of(node)  # Computed first time
ty = queries.type_of(node)  # Returned from cache
```

---

## 2. Crystal Compiler ⭐⭐⭐⭐ MOST RELEVANT

### Architecture: Program Container + Visitor Pattern

```
Program (top-level container)
  ├── Parser → AST
  ├── Normalizer → Normalized AST
  ├── TypeVisitor (multi-dispatch)
  │   ├── Type/Method declarations
  │   ├── Binding system (dependency graph)
  │   └── Type propagation
  └── Codegen → LLVM IR
```

### Key Components

**Program**: Top-level container holding:
- Basic types (Object, Nil, String, etc.)
- All classes and methods
- Compilation context

**TypeVisitor**: Implements Visitor Pattern with three concerns:
1. Type and method declarations
2. AST node binding for type propagation
3. Variable type analysis

### Binding System (Revolutionary)

```crystal
# When you bind node A to B, A's type gets B's type
a.bind(b)  # a observes b

# Type changes propagate through dependency graph
b.type = Int32  # → a.type automatically becomes Int32
```

**Implementation**:
- Nodes maintain dependencies and observers
- Type changes trigger propagation
- Automatic type inference via dataflow

### Design Philosophy

> "The design deliberately mixes concerns during type inference for compiler speed, prioritizing end-user performance over code clarity."

**Pragmatic approach**: Speed > Purity

### Applicability to MLC

**VERY HIGH** - Crystal is Ruby-like with static typing (same as MLC goals):

```ruby
# Proposed Binding System for MLC Phase 28
class IRNode
  attr_reader :type, :dependencies, :observers

  def type=(new_type)
    @type = new_type
    notify_observers  # Propagate to dependents
  end

  def bind(other_node)
    @dependencies << other_node
    other_node.add_observer(self)
  end

  private

  def notify_observers
    @observers.each { |obs| obs.update_from(self) }
  end
end

# Usage
then_node.bind(if_node)  # if_node updates when then_node changes
```

---

## 3. Swift Compiler ⭐⭐⭐⭐

### Architecture: Multi-Level IR

```
Swift Source → AST → SIL (Swift Intermediate Language) → LLVM IR
                      ↓
              Raw SIL → Canonical SIL
                        (optimizations)
```

### SIL (Swift Intermediate Language)

**Design Goals**:
- High-level, Swift-specific representation
- Preserves type information for optimization
- Language-specific optimizations difficult at LLVM IR level

**Two Flavors**:

1. **Raw SIL**: Direct lowering from type-checked AST
2. **Canonical SIL**: After mandatory optimization passes

### Benefits

- High-level optimizations (escape analysis, devirtualization)
- Type-preserving transformations
- Swift-specific patterns (protocols, generics)

### Applicability to MLC

**MEDIUM** - Good IR design principles:

```ruby
# Proposed Multi-Level IR for MLC Phase 29
AST → RawIR → CanonicalIR → LowIR → LLVM IR
      ↓         ↓             ↓
   (typing) (optimizations) (lowering)
```

**RawIR**: Direct AST mapping, all types preserved
**CanonicalIR**: Normalized, simplified, optimized
**LowIR**: Close to machine, generics erased

---

## 4. OCaml Compiler ⭐⭐⭐

### Architecture: Typed AST + Lambda Form

```
Source → Untyped AST → Typed AST → Lambda Form → Bytecode/Native
                       ↓
                   Type Checker
```

### Lambda Form

**Key transformation**: Type erasure

- Eliminates modules, objects, high-level constructs
- Replaces with records and function pointers
- Pattern matching compiled to decision trees
- Maps source to runtime memory model

### Benefits

- Clear separation: typed frontend, untyped backend
- Pattern match optimization at Lambda level
- Bytecode and native share Lambda backend

### Applicability to MLC

**MEDIUM** - Type erasure concept useful for LowIR phase

---

## 5. LLVM ⭐⭐⭐

### Architecture: Pass Infrastructure

```
LLVM IR → Pass Manager → Optimized IR
          ↓
      Pass1 → Pass2 → Pass3 → ...
      (each pass transforms IR)
```

### Analysis vs Transformation Passes

- **Analysis passes**: Read-only, compute information
- **Transformation passes**: Modify IR

### Pass Dependencies

```cpp
class MyPass : public FunctionPass {
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.setPreservesAll();
  }
};
```

### Limitations

- Pass-based architecture is dated (Rust moved away)
- Not incremental by design
- Sequential execution (harder to parallelize)

### Applicability to MLC

**LOW** - We already have better architecture (Rule System + PassManager)

---

## Comparative Analysis

| Compiler | Language | Architecture | IR Levels | Incremental | Docs Quality |
|----------|----------|--------------|-----------|-------------|--------------|
| **Rust** | Rust | Query System | HIR, MIR, LLVM | ✅ Yes | ⭐⭐⭐⭐⭐ |
| **Crystal** | Crystal | Visitor + Binding | AST, Normalized, LLVM | ❌ No | ⭐⭐⭐ |
| **Swift** | Swift | Pass-based | AST, Raw SIL, Canonical SIL | ❌ No | ⭐⭐⭐⭐ |
| **OCaml** | OCaml | Pass-based | Untyped AST, Typed AST, Lambda | ❌ No | ⭐⭐⭐⭐ |
| **MLC** | Ruby | Rule-based | AST, HighIR | ❌ No | ⭐⭐ |

---

## Recommendations for MLC

### Phase 27: Query System (Rust-inspired)

**Goal**: Replace PassManager with Query-based architecture

**Benefits**:
- Demand-driven compilation (compute only what's needed)
- Automatic memoization (free caching)
- Foundation for incremental compilation

**Implementation**:

```ruby
module MLC
  module Queries
    # Base query class with memoization
    class Query
      @@cache = {}

      def self.call(*args)
        key = [name, args]
        @@cache[key] ||= execute(*args)
      end

      def self.execute(*args)
        raise NotImplementedError
      end
    end

    # Example queries
    class TypeOf < Query
      def self.execute(node)
        TypeInferenceService.infer_type(node)
      end
    end

    class TransformNode < Query
      def self.execute(node)
        ty = TypeOf.call(node)  # Calls another query (cached!)
        IRGen.transform(node, ty)
      end
    end
  end
end
```

### Phase 28: Binding System (Crystal-inspired)

**Goal**: Automatic type propagation via dependency graph

**Benefits**:
- Declarative type flow (no manual propagation)
- Observer pattern for updates
- Simpler type inference

**Implementation**:

```ruby
module MLC
  class IRNode
    attr_accessor :type
    attr_reader :dependencies, :observers

    def initialize
      @dependencies = []
      @observers = []
      @type = nil
    end

    def bind(other_node)
      @dependencies << other_node
      other_node.observers << self
      update_from(other_node) if other_node.type
    end

    def type=(new_type)
      return if @type == new_type
      @type = new_type
      @observers.each { |obs| obs.update_from(self) }
    end

    def update_from(node)
      self.type = node.type if node.type && !@type
    end
  end
end
```

### Phase 29: Multi-Level IR (Swift-inspired)

**Goal**: Split HighIR into Raw/Canonical, add LowIR

**Structure**:

```
AST → RawIR → CanonicalIR → LowIR → LLVM IR
      ↓         ↓             ↓
   (typing) (optimizations) (lowering)
```

**RawIR**: Direct AST mapping
- All types preserved
- High-level constructs intact
- Pattern matching as AST nodes

**CanonicalIR**: Normalized form
- Simplified control flow
- Pattern matching lowered to decision trees
- Dead code eliminated
- Constant folding done

**LowIR**: Close to machine
- Generics erased
- Records flattened
- Function calls direct (no dispatch)

---

## Ideal Architecture for MLC

```
┌─────────────────────────────────────────────────────────┐
│ Layer 1: Query System (Rust-inspired)                  │
│ - Demand-driven compilation                             │
│ - Memoization cache                                     │
│ - Incremental recompilation                             │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│ Layer 2: Compilation Context (Crystal-inspired)        │
│ - Program (top-level container)                         │
│ - Binding System (dependency graph)                     │
│ - Type propagation observers                            │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│ Layer 3: IR Pipeline (Swift-inspired)                  │
│ AST → RawIR → CanonicalIR → LowIR → LLVM              │
│      (typed) (optimized)  (lowered)                     │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│ Layer 4: Rule System (MLC current - KEEP!)             │
│ - Flexible dispatch                                     │
│ - Easy to extend                                        │
│ - Better than Visitor pattern                           │
└─────────────────────────────────────────────────────────┘
```

---

## Resources

### Rust
- **Rust Compiler Dev Guide**: https://rustc-dev-guide.rust-lang.org/
- **Query System**: https://rustc-dev-guide.rust-lang.org/query.html
- **Incremental Compilation**: https://rustc-dev-guide.rust-lang.org/queries/incremental-compilation-in-detail.html

### Crystal
- **Compiler Internals**: https://github.com/crystal-lang/crystal/wiki/Compiler-internals
- **Binding System**: Type propagation via observers
- **Visitor Pattern**: Multi-dispatch implementation

### Swift
- **SIL Documentation**: https://github.com/apple/swift/blob/main/docs/SIL.rst
- **Compiler Overview**: https://www.swift.org/documentation/swift-compiler/
- **Multi-level IR**: Raw SIL vs Canonical SIL

### OCaml
- **Real World OCaml - Compiler Backend**: https://dev.realworldocaml.org/compiler-backend.html
- **Lambda Form**: Type erasure and lowering
- **Pattern Match Compilation**: Decision tree generation

---

## Conclusion

**No single "perfect" compiler**, but clear patterns emerge:

1. **Query System** (Rust) = Modern, efficient, incremental-ready
2. **Binding System** (Crystal) = Automatic type propagation, elegant
3. **Multi-Level IR** (Swift) = Language-specific optimizations
4. **Rule System** (MLC current) = Better than Visitor pattern, KEEP IT!

**Next Steps**:
- Complete Phase 21-23 (cleanup, eliminate coupling)
- Implement Phase 27 (Query System foundation)
- Implement Phase 28 (Binding System)
- Design Phase 29 (Multi-level IR)
