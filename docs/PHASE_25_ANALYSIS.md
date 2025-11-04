# Phase 25-A Analysis: The Transform Methods Problem

## User's Correct Observation

> "какой смысл переносить всё просто в irgen, если методы остались те же?"
> "в чём суть тогда паттерна visitor, если полно методов transform осталось?"

**You are absolutely right!** Phase 25-A was incomplete. We only moved code but didn't fix the fundamental architectural problem.

## Current Situation (After Phase 25-A)

### What We Have

```ruby
# lib/mlc/irgen.rb (lines 263-267)
def transform_expression(expr)
  # Delegate to ExpressionVisitor instance (Phase 23-A)
  @expression_visitor.visit(expr)
end
```

**Problem**: This is just a wrapper! `transform_expression` calls `@expression_visitor.visit()`. It's a useless indirection layer.

### Who Calls transform_* Methods?

All calls come from **Rule classes** using the anti-pattern:

```ruby
# lib/mlc/rules/irgen/expression/call_rule.rb
callee = transformer.send(:transform_expression, node.callee)
args = node.args.map { |arg| transformer.send(:transform_expression, arg) }
```

### Architecture Layers (Current)

```
Rule classes
    ↓ (transformer.send(:transform_expression, ...))
IRGen.transform_expression() [USELESS WRAPPER]
    ↓ (@expression_visitor.visit(...))
ExpressionVisitor.visit()
    ↓
ExpressionVisitor.visit_call()
```

**Two layers doing nothing!**

## Real Visitor Pattern (Rust, Swift, LLVM)

### Rust Compiler (rustc)

```rust
// No transform_* methods in main compiler!
impl<'tcx> Visitor<'tcx> for MyVisitor {
    fn visit_expr(&mut self, expr: &'tcx Expr) {
        // Direct processing
        match expr.kind {
            ExprKind::Call(ref callee, ref args) => {
                self.visit_expr(callee);  // Direct recursive call
                for arg in args {
                    self.visit_expr(arg);
                }
            }
            // ...
        }
    }
}
```

**Key differences:**
- ✅ No `transform_*` wrapper methods
- ✅ Visitor directly handles recursion
- ✅ Pattern matching drives dispatch
- ✅ No `.send()` anti-pattern

### Swift Compiler

```swift
class ASTVisitor {
    func visit(_ expr: Expr) -> Result {
        // Direct dispatch based on expr type
        switch expr {
        case let call as CallExpr:
            return visitCall(call)
        case let binary as BinaryExpr:
            return visitBinary(binary)
        }
    }
}
```

**Key differences:**
- ✅ No wrapper methods
- ✅ Type-based dispatch
- ✅ Clean recursion

### LLVM (Pass Infrastructure)

LLVM doesn't use classical visitor - it uses **Pass** system:

```cpp
struct MyPass : public FunctionPass {
    bool runOnFunction(Function &F) override {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                // Direct iteration, no visitor
            }
        }
    }
};
```

## Our Problems

### 1. Wrapper Methods Serve No Purpose

```ruby
def transform_expression(expr)
  @expression_visitor.visit(expr)  # Just delegates!
end
```

Could be replaced with:

```ruby
# Direct call
@expression_visitor.visit(expr)
```

### 2. Rule Classes Use Anti-Pattern

```ruby
# Current (BAD)
transformer.send(:transform_expression, node.callee)

# Should be (GOOD)
@expression_visitor.visit(node.callee)
```

### 3. Hybrid Confusion

We have BOTH:
- Visitor classes (ExpressionVisitor, StatementVisitor)
- Transform methods (transform_expression, transform_statement)
- Rules (that use transformer.send())

**Too many layers!**

## Solutions

### Option A: Complete Visitor Pattern (Recommended)

**Remove all transform_* wrappers. Use visitors directly.**

1. **Remove transform_expression, transform_statement from IRGen**
2. **Update all Rule classes to access visitor directly**:
   ```ruby
   # Old
   transformer.send(:transform_expression, node.expr)

   # New
   context.fetch(:expression_visitor).visit(node.expr)
   ```
3. **Pass visitor instances in rule context**:
   ```ruby
   context = {
     transformer: self,
     expression_visitor: @expression_visitor,
     statement_visitor: @statement_visitor,
     # ...
   }
   ```

**Benefits:**
- ✅ Clean architecture like Rust/Swift
- ✅ No wrapper methods
- ✅ Visitors do actual work
- ✅ Eliminates `.send()` anti-pattern

**Effort**: ~4-8 hours to update all rules

### Option B: Rule-Based Architecture (Alternative)

**Keep rules, but eliminate visitors. Make IRGen methods do actual work.**

1. **Delete visitor classes**
2. **Make transform_* methods actual transformers (not wrappers)**
3. **Rules call transform_* directly (no .send())**

**Benefits:**
- ✅ Simpler (one pattern, not two)
- ✅ Rules remain high-level

**Drawbacks:**
- ❌ Not true visitor pattern
- ❌ Harder to extend

### Option C: Rewrite from Scratch

**Study rustc_ast_lowering and start fresh with clean visitor.**

**Benefits:**
- ✅ Clean architecture from day one
- ✅ Learn from mature compilers

**Drawbacks:**
- ❌ 2-3 weeks of work
- ❌ High risk of breaking everything

## Recommended Action: Option A (Incremental Cleanup)

### Step-by-Step Plan

#### Phase 25-B: Pass Visitors to Rules

1. Update rule context to include visitors:
   ```ruby
   context = {
     transformer: self,
     expression_visitor: @expression_visitor,
     statement_visitor: @statement_visitor,
     # ... existing fields
   }
   ```

2. Update ONE rule to use visitor directly (proof of concept)
3. Run tests
4. Update remaining rules systematically

#### Phase 25-C: Remove Transform Wrappers

1. After all rules use visitors directly
2. Delete `transform_expression`, `transform_statement` methods
3. Grep to ensure no callers remain
4. Run tests

#### Phase 25-D: Audit Remaining Transform Methods

Some transform_* methods do real work (not wrappers):
- `transform_block` - manages scope, has logic
- `transform_for_statement` - manages loop scope
- `transform_if_statement` - type checking

**Decision for each:**
- If simple wrapper → delete it, use visitor
- If complex logic → keep it, but rename to `build_*` or `process_*`
- If scope management → extract to ScopeManager service

## Comparison: Before vs After

### Before (Current - BAD)

```ruby
# Rule
transformer.send(:transform_expression, node.expr)

# IRGen (wrapper)
def transform_expression(expr)
  @expression_visitor.visit(expr)
end

# Visitor (actual work)
def visit_call(node)
  # ...
end
```

**3 layers for one operation!**

### After (Option A - GOOD)

```ruby
# Rule
context.fetch(:expression_visitor).visit(node.expr)

# Visitor (actual work)
def visit_call(node)
  # ...
end
```

**1 layer. Clean.**

## Conclusion

You are correct - Phase 25-A was incomplete. We need to:

1. **Pass visitors directly to rules** (no wrapper methods)
2. **Eliminate transform_* wrappers** that just delegate
3. **Audit remaining transform_* methods** - keep only those with real logic
4. **Follow Rust/Swift pattern** - visitors do the work, no extra layers

This will take ~1-2 days but results in clean, maintainable architecture.

**Question for you**: Should we proceed with Option A (incremental cleanup) or do you prefer Option C (rewrite from scratch with rustc as reference)?
