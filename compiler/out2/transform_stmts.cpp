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

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept;

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept{
mlc::Array<std::shared_ptr<semantic_ir::SStmt>> typed_statements = {};
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> current_env = transform_context.type_env;
int statement_index = 0;
while (statement_index < statements.size()){
{
transform::TransformContext current_context = transform::transform_context_with_env(transform_context, current_env);
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [binding_name, is_mut, annotation, value_expression, source_span] = stmtlet; return [&]() -> std::tuple<> { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation(annotation);
  std::shared_ptr<semantic_ir::SExpr> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  current_env.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SExpr> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet(binding_name, is_mut, coerced_value, value_type, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtLetPat& stmtletpat) -> std::tuple<> { auto [pattern, is_mut, annotation, value_expression, source_span] = stmtletpat; return [&]() -> std::tuple<> { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation(annotation);
  std::shared_ptr<semantic_ir::SExpr> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  std::shared_ptr<semantic_ir::SExpr> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  let_pattern_infer::infer_let_pattern_env(pattern, value_type, current_env, current_context.registry);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetPat(pattern, is_mut, coerced_value, value_type, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [binding_name, annotation, value_expression, source_span] = stmtletconst; return [&]() -> std::tuple<> { 
  std::shared_ptr<registry::Type> annotated_type = registry::type_from_annotation(annotation);
  std::shared_ptr<semantic_ir::SExpr> typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
  std::shared_ptr<registry::Type> inferred_type = semantic_ir::sexpr_type(typed_value);
  std::shared_ptr<registry::Type> value_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TUnit>((*annotated_type))) {  return inferred_type; } return annotated_type; }();
  current_env.set(binding_name, value_type);
  std::shared_ptr<semantic_ir::SExpr> coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst(binding_name, coerced_value, value_type, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, source_span] = stmtexpr; return [&]() -> std::tuple<> { 
  std::shared_ptr<semantic_ir::SExpr> typed_expr = transform::transform_expr(expression, current_context, transform_stmts);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr(typed_expr, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [return_expression, source_span] = stmtreturn; return [&]() -> std::tuple<> { 
  std::shared_ptr<semantic_ir::SExpr> typed_return = transform::transform_expr(return_expression, current_context, transform_stmts);
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtReturn(typed_return, source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [source_span] = stmtbreak; return [&]() -> std::tuple<> { 
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtBreak(source_span)));
  return std::make_tuple();
 }(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [source_span] = stmtcontinue; return [&]() -> std::tuple<> { 
  typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtContinue(source_span)));
  return std::make_tuple();
 }(); }
}, (*statements[statement_index])._);
statement_index = statement_index + 1;
}
}
return transform::TransformStmtsResult{typed_statements, current_env};
}

} // namespace transform_stmts
