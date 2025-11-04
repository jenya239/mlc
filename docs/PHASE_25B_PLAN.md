# Phase 25-B Implementation Plan: Pass Visitors to Rules

## Goal

Remove `transformer.send(:transform_expression/statement)` anti-pattern by passing visitors directly to rules.

## Current Architecture (BAD)

```ruby
# Rule
callee = transformer.send(:transform_expression, node.callee)

# IRGen wrapper (USELESS!)
def transform_expression(expr)
  @expression_visitor.visit(expr)
end

# Visitor (actual work)
def visit_call(node)
  # ...
end
```

**3 layers for nothing!**

## Target Architecture (GOOD)

```ruby
# Rule
callee = context.fetch(:expression_visitor).visit(node.callee)

# Visitor (actual work)
def visit_call(node)
  # ...
end
```

**1 layer. Clean.**

## Implementation Steps

### Step 1: Add Visitors to Context ✅ NEXT

**Files to modify:**
- `lib/mlc/irgen.rb` - `apply_expression_rules()` method
- `lib/mlc/irgen.rb` - `apply_statement_rules()` method

**Changes:**
```ruby
# In apply_expression_rules (line ~271)
context = {
  transformer: self,
  expression_visitor: @expression_visitor,  # NEW
  statement_visitor: @statement_visitor,    # NEW
  # ... rest of context
}

# In apply_statement_rules (line ~352)
context = {
  transformer: self,
  expression_visitor: @expression_visitor,  # NEW
  statement_visitor: @statement_visitor,    # NEW
  # ... rest of context
}
```

**Why both visitors in both contexts?**
- Statement rules may need to transform expressions (e.g., `if condition`)
- Expression rules may need to transform statement blocks (e.g., `if-then-else`)

**Risk**: LOW - Just adding new keys, backward compatible

**Test**: `rake test` - should pass unchanged

### Step 2: Update ONE Rule (Proof of Concept)

**Target rule**: `lib/mlc/rules/irgen/expression/call_rule.rb`

**Why CallRule?**
- Simple and straightforward
- Clear transformation pattern
- Good representative example

**Current code (lines ~23-24)**:
```ruby
callee = transformer.send(:transform_expression, node.callee)
args = node.args.map { |arg| transformer.send(:transform_expression, arg) }
```

**New code**:
```ruby
expression_visitor = context.fetch(:expression_visitor)
callee = expression_visitor.visit(node.callee)
args = node.args.map { |arg| expression_visitor.visit(arg) }
```

**Risk**: LOW - CallRule is well-tested

**Test**: Run expression tests:
```bash
ruby -Ilib:test test/mlc/aurora_language_test.rb -n test_function_call
```

### Step 3: Update Remaining Expression Rules

**Files** (9 rules total):
1. ✅ `lib/mlc/rules/irgen/expression/call_rule.rb` (DONE in Step 2)
2. `lib/mlc/rules/irgen/expression/if_rule.rb`
3. `lib/mlc/rules/irgen/expression/lambda_rule.rb`
4. `lib/mlc/rules/irgen/expression/let_rule.rb`
5. `lib/mlc/rules/irgen/expression/list_comprehension_rule.rb`
6. `lib/mlc/rules/irgen/expression/member_rule.rb`
7. `lib/mlc/rules/irgen/expression/while_loop_rule.rb`
8. `lib/mlc/rules/irgen/expression/do_rule.rb`
9. `lib/mlc/rules/irgen/expression/for_loop_rule.rb`

**Process for each:**
1. Find all `transformer.send(:transform_expression, ...)` calls
2. Replace with `context.fetch(:expression_visitor).visit(...)`
3. Find all `transformer.send(:transform_statement_block, ...)` calls
4. Replace with direct visitor call or keep wrapper (TBD)
5. Run tests after EACH rule update

**Risk**: MEDIUM - Many rules, but pattern is consistent

**Test**: `rake test` after each rule

### Step 4: Update Statement Rules

**Files** (8 rules):
1. `lib/mlc/rules/irgen/statement/return_rule.rb`
2. `lib/mlc/rules/irgen/statement/variable_decl_rule.rb`
3. `lib/mlc/rules/irgen/statement/assignment_rule.rb`
4. `lib/mlc/rules/irgen/statement/if_rule.rb`
5. `lib/mlc/rules/irgen/statement/while_rule.rb`
6. `lib/mlc/rules/irgen/statement/for_rule.rb`
7. `lib/mlc/rules/irgen/statement/expr_stmt_rule.rb`
8. `lib/mlc/rules/irgen/statement/block_rule.rb`

**Same process as Step 3**

**Risk**: MEDIUM

**Test**: `rake test` after each rule

### Step 5: Handle transform_statement_block

**Problem**: Many rules call `transformer.send(:transform_statement_block, ...)`

**Options:**

**Option A: Keep wrapper temporarily**
- Leave `transform_statement_block` in IRGen for now
- Rules call it directly (no `.send()`)
- Clean up in Phase 25-D

**Option B: Use visitor directly**
- StatementVisitor needs `visit_block` method
- Rules call `statement_visitor.visit(block_ast)`

**Decision**: Option A for now (safer)

### Step 6: Remove transform_expression Wrapper

After ALL rules use visitor directly:

```ruby
# DELETE this method from lib/mlc/irgen.rb
def transform_expression(expr)
  @expression_visitor.visit(expr)
end
```

**Verification**:
```bash
grep -r "transformer.send(:transform_expression" lib/mlc/rules
# Should return NOTHING
```

**Test**: `rake test` - should pass

### Step 7: Audit Remaining transform_* Methods

**Keep these (they do real work)**:
- `transform_block` - scope management
- `transform_for_statement` - loop scope management
- `transform_if_statement` - type checking logic
- `transform_while_statement` - loop scope management
- `transform_variable_decl_statement` - type inference
- `transform_assignment_statement` - type checking

**Consider renaming to `process_*` or `build_*` to avoid confusion**

**Delete these (just wrappers)**:
- `transform_expression` ✅
- `transform_statement` (if exists)

## Testing Strategy

### After Each Step

```bash
# Quick smoke test
rake test

# Full test suite
rake test TESTOPTS="--verbose"
```

### Final Verification

```bash
# No more transformer.send() calls in rules
grep -r "transformer.send" lib/mlc/rules
# Should be EMPTY or only non-transform methods

# All tests passing
rake test
```

## Estimated Effort

- **Step 1** (Add visitors to context): 15 minutes
- **Step 2** (Proof of concept - CallRule): 30 minutes
- **Step 3** (Expression rules): 2-3 hours
- **Step 4** (Statement rules): 2-3 hours
- **Step 5** (Handle statement_block): 1 hour
- **Step 6** (Remove wrappers): 30 minutes
- **Step 7** (Audit remaining): 1 hour

**Total**: ~8-10 hours (1-2 days)

## Rollback Plan

If anything breaks:
```bash
git diff HEAD lib/mlc/rules > rules_changes.patch
git checkout HEAD -- lib/mlc/rules
git checkout HEAD -- lib/mlc/irgen.rb
```

## Success Criteria

1. ✅ All tests passing
2. ✅ Zero `transformer.send(:transform_expression)` calls in rules
3. ✅ Zero `transformer.send(:transform_statement)` calls in rules
4. ✅ Visitors passed via context, not wrappers
5. ✅ Code cleaner, easier to understand

## Next Steps After 25-B

- **Phase 25-C**: Audit and potentially remove/rename remaining transform_* methods
- **Phase 25-D**: Further cleanup based on lessons learned
- **Future**: Study rustc_ast_lowering for architectural insights
