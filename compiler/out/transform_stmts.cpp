#include "transform_stmts.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "transform.hpp"
#include "let_pattern_infer.hpp"

namespace transform_stmts {

using namespace ast;
using namespace registry;
using namespace semantic_ir;
using namespace transform;
using namespace let_pattern_infer;
using namespace ast_tokens;

struct Transform_stmts_fold_state {mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> typed_statements;mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment;};

transform_stmts::Transform_stmts_fold_state transform_stmts_fold_step(transform_stmts::Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement, transform::TransformContext transform_context) noexcept;

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

transform_stmts::Transform_stmts_fold_state transform_stmts_fold_step(transform_stmts::Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement, transform::TransformContext transform_context) noexcept{
transform::TransformContext current_context = transform::transform_context_with_env(transform_context, accumulator.type_environment);
return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> transform_stmts::Transform_stmts_fold_state { auto [binding_name, is_mut, annotation, value_expression, source_span] = stmtlet; return [&]() -> transform_stmts::Transform_stmts_fold_state { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
  std::shared_ptr<semantic_ir::SemanticExpression> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = accumulator.type_environment;
  type_environment.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SemanticExpression> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet(binding_name, is_mut, coerced_value, value_type, source_span))}), type_environment};
 }(); },
  [&](const StmtLetPattern& stmtletpattern) -> transform_stmts::Transform_stmts_fold_state { auto [pattern, is_mut, annotation, value_expression, has_else, else_body, source_span] = stmtletpattern; return [&]() -> transform_stmts::Transform_stmts_fold_state { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
  std::shared_ptr<semantic_ir::SemanticExpression> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  std::shared_ptr<semantic_ir::SemanticExpression> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = accumulator.type_environment;
  let_pattern_infer::infer_let_pattern_env(pattern, value_type, type_environment, current_context.registry);
  std::shared_ptr<semantic_ir::SemanticExpression> typed_else_body = transform::transform_expr(else_body, current_context, transform_stmts);
  return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern(pattern, is_mut, coerced_value, value_type, has_else, typed_else_body, source_span))}), type_environment};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> transform_stmts::Transform_stmts_fold_state { auto [binding_name, annotation, value_expression, source_span] = stmtletconst; return [&]() -> transform_stmts::Transform_stmts_fold_state { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
  std::shared_ptr<semantic_ir::SemanticExpression> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment = accumulator.type_environment;
  type_environment.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SemanticExpression> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst(binding_name, coerced_value, value_type, source_span))}), type_environment};
 }(); },
  [&](const StmtExpr& stmtexpr) -> transform_stmts::Transform_stmts_fold_state { auto [expression, source_span] = stmtexpr; return [&]() -> transform_stmts::Transform_stmts_fold_state { 
  std::shared_ptr<semantic_ir::SemanticExpression> typed_expr = transform::transform_expr(expression, current_context, transform_stmts);
  return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr(typed_expr, source_span))}), accumulator.type_environment};
 }(); },
  [&](const StmtReturn& stmtreturn) -> transform_stmts::Transform_stmts_fold_state { auto [return_expression, source_span] = stmtreturn; return [&]() -> transform_stmts::Transform_stmts_fold_state { 
  std::shared_ptr<semantic_ir::SemanticExpression> typed_return = transform::transform_expr(return_expression, current_context, transform_stmts);
  return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementReturn(typed_return, source_span))}), accumulator.type_environment};
 }(); },
  [&](const StmtBreak& stmtbreak) -> transform_stmts::Transform_stmts_fold_state { auto [source_span] = stmtbreak; return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementBreak(source_span))}), accumulator.type_environment}; },
  [&](const StmtContinue& stmtcontinue) -> transform_stmts::Transform_stmts_fold_state { auto [source_span] = stmtcontinue; return transform_stmts::Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>>{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementContinue(source_span))}), accumulator.type_environment}; }
}, (*statement)._);
}

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept{
transform_stmts::Transform_stmts_fold_state final_state = statements.fold(transform_stmts::Transform_stmts_fold_state{{}, transform_context.type_env}, [transform_context](transform_stmts::Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement) mutable { return transform_stmts_fold_step(accumulator, statement, transform_context); });
return transform::TransformStmtsResult{final_state.typed_statements, final_state.type_environment};
}

} // namespace transform_stmts
