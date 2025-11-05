# IRGen v2 Migration — Progress Log (2025‑11‑02)

## Completed

- **Match expressions (v2)**  
  - Introduced `MatchService` with shared helpers for pattern binding, type checks, and IR construction.  
  - Engine/container now wire match analyser + unification service into the v2 pipeline.  
  - Declarative `MatchRule` delegates entirely to services; visitors stay free of node-class checks.

- **Match statements (v2)**  
  - `MatchService` promotes eligible expression matches (unit arms, no guards) into `HighIR::MatchStmt` wrapped in a block.  
  - Statement arms reuse rule-engine visitors; builders gained explicit `match_stmt`.  
  - Regression test added to cover the statement-form lowering.

- **Pattern feature parity**  
  - Constructor arms now expose `bindings` data for downstream codegen; binding types flow through the variable registry.  
  - Regex patterns include capture bindings and seed the registry with string types, enabling captures inside bodies.

- **Member access parity**  
  - New MemberAccessService resolves record, array, string, and numeric members without referencing AST classes.  
  - Declarative MemberRule covers non-module accesses; module detection stays in ModuleMemberRule.  
  - Regression tests ensure record fields, array helpers, and string utilities return expected HighIR types.

- **Index access support**  
  - `IndexAccessService` validates array index operations and produces `HighIR::IndexExpr`.  
  - Rule/visitor wiring keeps array indexing declarative; new test covers typed element lookup.

- **AST abstraction hygiene**  
  - Loop/while/assignment/return rules route all node-shape checks through `ASTTypeChecker`.  
  - `ASTFactory` exposes pattern helpers (`match_arm`, `pattern_var`, etc.) so specs and rules avoid class names.

## Tests

```
bundle exec ruby -Ilib -Itest test/mlc/irgen_v2/engine_test.rb
```

## Next Focus

1. Sweep remaining v2 rules for direct AST class references or duplicated traversal logic; move into dedicated services.  
2. Expand `MatchService` utilities (constructor field destructuring, regex support) to match legacy feature parity.  
3. Once coverage matches v1, delete legacy `PatternMatchingTransformer` and the old rule registrations.
