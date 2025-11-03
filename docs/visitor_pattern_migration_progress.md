# Visitor Pattern Migration Progress

## Session: 2025-11-03 - Continuing Phase 23 Migration

### Overview

User requested to complete the full migration from transformers to visitor pattern. This builds on Phase 23-C work that introduced visitor infrastructure but left some rules incompletely migrated.

### Initial Investigation

**Test Status at Start**:
- 1474 runs, 3794 assertions
- 13 failures + 14 errors = **27 issues**

**Key Finding**: Visitor pattern infrastructure was partially implemented, but had critical bugs:

1. **StatementVisitor bug** (lib/mlc/visitors/statement_visitor.rb:114):
   - Used `node.value` but AST::Return has `attr_reader :expr`
   - This was an AST attribute name mismatch

2. **ReturnRule not using visitor-provided IR** (lib/mlc/rules/irgen/statement/return_rule.rb:31):
   - Visitor passed pre-transformed `value_ir` in context
   - But ReturnRule ignored it and did own transformation via `transformer.send()`

3. **Dual-Path Pattern** - Discovered that Phase 23-C implemented a dual-path architecture:
   - **New path**: When `context.key?(:value_ir)` is true, use visitor-provided IR
   - **Legacy path**: For backward compatibility, fallback to inline transformation
   - Examples: AssignmentRule, VariableDeclRule, ExprStmtRule

### Fixes Implemented

#### 1. StatementVisitor AST Attribute Fix
**File**: `lib/mlc/visitors/statement_visitor.rb:114`

**Before**:
```ruby
def visit_return(node)
  value_ir = node.value ? @expression_visitor.visit(node.value) : nil
  # ...
end
```

**After**:
```ruby
def visit_return(node)
  value_ir = node.expr ? @expression_visitor.visit(node.expr) : nil
  # ...
end
```

**Impact**: Visitor now correctly accesses the `expr` attribute of Return nodes.

#### 2. ReturnRule Dual-Path Pattern
**File**: `lib/mlc/rules/irgen/statement/return_rule.rb:24-30`

**Before**:
```ruby
# Transform return value expression if present
value_ir = node.expr ? transformer.send(:transform_expression, node.expr) : nil
```

**After**:
```ruby
# Phase 23-C: Support both visitor path (with value_ir) and legacy path (without)
value_ir = if context.key?(:value_ir)
             # New path: Visitor provides pre-transformed IR
             context.fetch(:value_ir)
           else
             # Legacy path: Transform inline for backward compatibility
             node.expr ? transformer.send(:transform_expression, node.expr) : nil
           end
```

**Impact**: ReturnRule now follows the same dual-path pattern as other migrated rules.

### Test Results After Fixes

- 1474 runs, 3794 assertions
- 12 failures + 11 errors = **23 issues**
- **Improvement: -4 issues (-15%)**

### Architecture Patterns Identified

#### Dual-Path Pattern (Phase 23-C Standard)

Rules that support visitor pattern use this pattern:

```ruby
def apply(node, context = {})
  transformer = context.fetch(:transformer)

  # Dual-path: Check if visitor provided pre-transformed IR
  value_ir = if context.key?(:value_ir)
               # New path: Use visitor-provided IR
               context.fetch(:value_ir)
             else
               # Legacy path: Transform inline
               transformer.send(:transform_expression, node.value)
             end

  # ... rest of rule logic uses value_ir ...
end
```

#### Statement Rules Status

| Rule | Dual-Path? | Notes |
|------|------------|-------|
| ReturnRule | ✅ NOW | Fixed in this session |
| AssignmentRule | ✅ | Already implemented |
| VariableDeclRule | ✅ | Already implemented (also uses transform_type) |
| ExprStmtRule | ✅ | Already implemented (complex statement type handling) |
| BreakRule | ✅ | No child nodes to transform |
| ContinueRule | ✅ | No child nodes to transform |
| WhileRule | ? | Needs verification |
| ForRule | ? | Needs verification |
| IfRule | ? | Needs verification |
| BlockRule | ❌ | Uses `transformer.send(:transform_block)` without dual-path |

#### Expression Rules Status

Most expression rules still use `transformer.send()` without dual-path pattern:
- CallRule
- LetRule
- LambdaRule
- ListComprehensionRule
- MemberRule
- IfRule (expression version)
- WhileLoopRule
- DoRule
- ForLoopRule

**Note**: Expression rules may need different handling since ExpressionVisitor architecture differs from StatementVisitor.

### Remaining Work

#### High Priority

1. **Verify other statement rules** (while, for, if):
   - Check if they properly implement dual-path pattern
   - Fix any that don't follow the pattern

2. **Fix BlockRule**:
   - Currently uses `transformer.send(:transform_block)` directly
   - Needs visitor pattern support or dual-path

#### Medium Priority

3. **Expression rules architecture**:
   - Understand how ExpressionVisitor handles expressions
   - Determine if expression rules need visitor support
   - Some expression rules may be intentionally legacy-only

4. **Eliminate remaining transformer.send() calls**:
   - 17 files still use the anti-pattern
   - Need to categorize: dual-path vs legacy-only vs needs-migration

#### Low Priority

5. **Documentation updates**:
   - Update Phase 23 documentation with findings
   - Document dual-path pattern as standard approach
   - Create migration guide for future rules

### Key Insights

1. **Phase 23-C was incomplete**: Infrastructure was added but not all rules were updated to use it.

2. **Dual-path pattern is critical**: Rules need backward compatibility during transition period.

3. **AST attribute mismatches**: Visitor code must match actual AST node attributes (caught by tests if visitor path is active).

4. **Statement vs Expression visitors**: Different architectures may require different migration strategies.

### grep Results: transformer.send() Usage

Rules still using `transformer.send()`:

**Statement rules:**
- lib/mlc/rules/irgen/statement/return_rule.rb (has dual-path now)
- lib/mlc/rules/irgen/statement/expr_stmt_rule.rb (has dual-path)
- lib/mlc/rules/irgen/statement/for_rule.rb (needs verification)
- lib/mlc/rules/irgen/statement/while_rule.rb (needs verification)
- lib/mlc/rules/irgen/statement/if_rule.rb (needs verification)
- lib/mlc/rules/irgen/statement/assignment_rule.rb (has dual-path)
- lib/mlc/rules/irgen/statement/variable_decl_rule.rb (has dual-path)
- lib/mlc/rules/irgen/statement/block_rule.rb (no dual-path)

**Expression rules:**
- lib/mlc/rules/irgen/expression/lambda_rule.rb
- lib/mlc/rules/irgen/expression/list_comprehension_rule.rb
- lib/mlc/rules/irgen/expression/call_rule.rb
- lib/mlc/rules/irgen/expression/member_rule.rb
- lib/mlc/rules/irgen/expression/if_rule.rb
- lib/mlc/rules/irgen/expression/while_loop_rule.rb
- lib/mlc/rules/irgen/expression/do_rule.rb
- lib/mlc/rules/irgen/expression/for_loop_rule.rb
- lib/mlc/rules/irgen/expression/let_rule.rb

### Next Steps

**Immediate**:
1. Check while_rule, for_rule, if_rule for dual-path pattern
2. Fix BlockRule to support visitor pattern
3. Run tests after each fix

**Follow-up**:
4. Investigate expression rule architecture
5. Create comprehensive migration plan for remaining rules
6. Update documentation

---

**Session End**: Partial migration completed, -4 test issues fixed. Visitor pattern infrastructure is working, but more rules need dual-path pattern implementation.
