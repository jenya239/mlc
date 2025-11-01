# ExpressionTransformer - Full Refactoring Plan

## Current Reality Check

**Current State:** 776 lines  
**Claim:** "Thin coordinator"  
**Reality:** Still has transformation logic!

### What's Inside (Full Breakdown)

```
776 lines total:
├─ 180 lines: 16 DEAD transform_* methods ❌
├─  50 lines: 2 LIVE transform_* (pipe, match) ⚠️  
├─  39 lines: Record builders (build_*, construct_*) ⚠️
├─  32 lines: Type unification logic ⚠️
├─  50 lines: Pattern/match support logic ⚠️
└─ 425 lines: Everything else
```

## Problem: "Worst of Both Worlds"

### What MLC Tried to Do (LLVM/MLIR Pattern)
```
Thin Coordinator + Rules
├─ Coordinator: Routes nodes to rules (~50 lines)
└─ Rules: Contain ALL logic (self-contained)
```

### What MLC Actually Has
```
Hybrid Mess
├─ Rules: Contain SOME logic
├─ Transformer: Contains OTHER logic
└─ Logic SPLIT between both!
```

**Example:**
- `CallRule` exists and has logic ✓
- But `transform_call()` also exists! ✗
- Which one is used? CallRule! ✓
- So `transform_call()` is dead code! ✗

## Three Options Forward

### Option 1: Full LLVM/MLIR Pattern (Recommended)
**Target:** ~50 lines for ExpressionTransformer

**Actions:**
1. ✅ Remove 16 dead `transform_*` methods (-180 lines)
2. Move `transform_pipe` logic → PipeRule 
3. Keep `transform_match_expr` (complex, used by MatchRule)
4. Move record builders → RecordBuilderService (already exists!)
5. Move type unification → TypeInferenceService (already exists!)
6. Move pattern bindings → MatchRule or MatchAnalyzer

**Result:**
```ruby
# lib/mlc/irgen/expression_transformer.rb (~50 lines)
module ExpressionTransformer
  def transform_expression(expr)
    result = apply_expression_rules(expr)
    
    # Special case: AST::Block
    return transform_block(expr) if result.equal?(expr) && expr.is_a?(AST::Block)
    
    # Error if no rule
    raise CompileError, "No rule..." if result.equal?(expr)
    
    result
  end
  
  def apply_expression_rules(expr)
    @rule_engine.apply(:core_ir_expression, expr, context: build_context)
  end
  
  private
  
  def build_context
    {
      transformer: self,
      type_registry: @type_registry,
      # ... services
    }
  end
end
```

**Benefits:**
- ✅ True thin coordinator
- ✅ All logic in rules/services
- ✅ Easy to test rules in isolation
- ✅ Follows LLVM/MLIR patterns
- ✅ Easy to add new expressions

**Effort:** 2-3 days

---

### Option 2: Rustc Monolithic Pattern
**Target:** ~1000 lines, NO rules

**Actions:**
1. Delete ALL rules
2. Move all logic back into ExpressionTransformer
3. Organize by concept (like rustc):
   - `lower_call_expr`
   - `lower_if_expr`
   - etc.

**Result:**
```ruby
# lib/mlc/irgen/expression_transformer.rb (~1000 lines)
module ExpressionTransformer
  def transform_expression(expr)
    case expr
    when AST::Call
      lower_call_expr(expr)
    when AST::IfExpr
      lower_if_expr(expr)
    # ... all cases here
    end
  end
  
  private
  
  def lower_call_expr(expr)
    # ALL call logic here (~50 lines)
  end
  
  # ... many more methods
end
```

**Benefits:**
- ✅ Simpler architecture (no rules framework)
- ✅ All logic in one place
- ✅ Like rustc/GCC (proven pattern)

**Drawbacks:**
- ❌ Loses rule extensibility
- ❌ Harder to add custom transformations
- ❌ Reverses Phase 2 service work

**Effort:** 1 week (rollback + reorganize)

---

### Option 3: Keep Current Hybrid (NOT Recommended)
**Target:** Status quo

**Actions:**
- None

**Reality:**
- ❌ Confusing architecture
- ❌ Logic split across files
- ❌ Dead code remains
- ❌ Not following any pattern

---

## Recommendation: Option 1

### Why?
1. **Already 90% there!** Rules exist, services exist
2. **Just need to finish the migration**
3. **Matches stated goals** (LLVM/MLIR patterns)
4. **Phase 2 service architecture supports this**

### Detailed Plan (Option 1)

#### Step 1: Remove Dead Code (SAFE, 30 min)
```bash
# Delete these 16 methods:
- transform_call
- transform_if_expr
- transform_let
- transform_lambda
- transform_record_literal
- transform_index_access
- transform_list_comprehension
- transform_for_loop
- transform_while_loop  
- transform_do_expr
- transform_array_literal
- transform_literal
- transform_var_ref
- transform_binary
- transform_unary
- transform_member_access
```

**Impact:** 776 → 596 lines  
**Risk:** None (not used anywhere)

#### Step 2: Move PipeRule Logic (1 hour)
Currently:
```ruby
# PipeRule delegates to transformer
class PipeRule
  def apply(node, context)
    context[:transformer].__send__(:transform_pipe, node)
  end
end
```

Change to:
```ruby
# PipeRule contains full logic
class PipeRule
  def apply(node, context)
    expr_svc = context[:expression_transformer]
    left = expr_svc.transform_expression(node.left)
    
    if node.right.is_a?(AST::Call)
      # ... full pipe logic here
    end
  end
end
```

**Impact:** 596 → 570 lines  
**Risk:** Low (move code, test)

#### Step 3: Move Record Builders → Service (2 hours)
Move to `RecordBuilderService` (already exists!):
- `build_anonymous_record`
- `build_named_record`
- `construct_record_from_info`
- `select_best_record_candidate`

**Impact:** 570 → 531 lines  
**Risk:** Low (service exists)

#### Step 4: Move Type Logic → Service (2 hours)
Move to `TypeInferenceService`:
- `unify_type`
- `type_equivalent`
- `infer_record_from_*`

**Impact:** 531 → 480 lines  
**Risk:** Low (service exists)

#### Step 5: Move Pattern Logic (3 hours)
Move to MatchAnalyzer or MatchRule:
- `bind_pattern_variables`
- `expected_lambda_param_types`
- `lambda_return_type`
- `match_stmt_applicable`
- `match_stmt_safe_pattern`

**Impact:** 480 → ~430 lines  
**Risk:** Medium (complex logic)

#### Step 6: Analyze Remaining Code
After steps 1-5, re-evaluate what's left.  
Target: Identify more logic that can move to rules/services.

**Goal:** Get to ~50-100 lines

---

## Comparison: Real Compilers

### LLVM/MLIR
```cpp
// mlir/lib/Dialect/SCF/Transforms/ForToWhile.cpp (130 lines)
// Each pass is small, focused, self-contained
```

### Rustc
```rust
// compiler/rustc_ast_lowering/src/expr.rs (1800 lines)
// Monolithic but organized, no rules framework
```

### TypeScript
```typescript
// src/compiler/transformer.ts (500 lines)
// Thin coordinator
// src/compiler/transformers/*.ts (many files)
// Actual transform logic
```

### MLC Current
```
lib/mlc/irgen/expression_transformer.rb (776 lines)
  ├─ Coordinator code
  ├─ Helper methods (some dead!)
  └─ Support logic

lib/mlc/rules/irgen/expression/*.rb (19 files)
  ├─ Some contain full logic ✓
  └─ Some delegate to transformer ✗
```

### MLC Target (Option 1)
```
lib/mlc/irgen/expression_transformer.rb (~50 lines)
  └─ Pure coordinator

lib/mlc/rules/irgen/expression/*.rb (19 files)
  └─ ALL contain full logic ✓

lib/mlc/services/*.rb (7 files)
  └─ Shared logic (type inference, builders, etc.)
```

---

## Decision Point

**Question:** Which option?

1. **Option 1:** Full LLVM pattern (recommended, 2-3 days)
2. **Option 2:** Rustc monolithic (simpler, 1 week, rollback)
3. **Option 3:** Keep hybrid (not recommended)

**My recommendation:** Option 1
- Already invested in rules/services
- Just need to finish the migration
- Will achieve true thin coordinator
- Matches architecture goals

**Next action:** Remove dead code (Step 1)?
