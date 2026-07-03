#define main mlc_user_main
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

Transform_stmts_fold_state transform_stmts_fold_step(Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement, transform::TransformContext transform_context) noexcept{
  auto current_context = transform::transform_context_with_env(transform_context, accumulator.type_environment);
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> Transform_stmts_fold_state { auto [binding_name, is_mut, annotation, value_expression, source_span] = stmtLet; return [&]() {
auto annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = annotated_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return inferred_type;
}
return annotated_type;
std::abort();
}();
auto type_environment = accumulator.type_environment;
type_environment.set(binding_name, value_type);
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLet{binding_name, is_mut, coerced_value, value_type, source_span})}), type_environment};
}(); },
[&](const ast::StmtLetPattern& stmtLetPattern) -> Transform_stmts_fold_state { auto [pattern, is_mut, annotation, value_expression, has_else, else_body, source_span] = stmtLetPattern; return [&]() {
auto annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = annotated_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return inferred_type;
}
return annotated_type;
std::abort();
}();
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
auto type_environment = accumulator.type_environment;
let_pattern_infer::infer_let_pattern_env(pattern, value_type, type_environment, current_context.registry);
auto typed_else_body = transform::transform_expr(else_body, current_context, transform_stmts);
return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetPattern{pattern, is_mut, coerced_value, value_type, has_else, typed_else_body, source_span})}), type_environment};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> Transform_stmts_fold_state { auto [binding_name, annotation, value_expression, source_span] = stmtLetConst; return [&]() {
auto annotated_type = registry::type_from_annotation_with_registry(annotation, current_context.registry);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = annotated_type;
if (std::holds_alternative<registry::TUnit>((*__match_subject))) {
const registry::TUnit& tUnit = std::get<registry::TUnit>((*__match_subject));
return inferred_type;
}
return annotated_type;
std::abort();
}();
auto type_environment = accumulator.type_environment;
type_environment.set(binding_name, value_type);
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementLetConst{binding_name, coerced_value, value_type, source_span})}), type_environment};
}(); },
[&](const ast::StmtExpr& stmtExpr) -> Transform_stmts_fold_state { auto [expression, source_span] = stmtExpr; return [&]() {
auto typed_expr = transform::transform_expr(expression, current_context, transform_stmts);
return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementExpr{typed_expr, source_span})}), accumulator.type_environment};
}(); },
[&](const ast::StmtReturn& stmtReturn) -> Transform_stmts_fold_state { auto [return_expression, source_span] = stmtReturn; return [&]() {
auto typed_return = transform::transform_expr(return_expression, current_context, transform_stmts);
return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementReturn{typed_return, source_span})}), accumulator.type_environment};
}(); },
[&](const ast::StmtBreak& stmtBreak) -> Transform_stmts_fold_state { auto [source_span] = stmtBreak; return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementBreak{source_span})}), accumulator.type_environment}; },
[&](const ast::StmtContinue& stmtContinue) -> Transform_stmts_fold_state { auto [source_span] = stmtContinue; return Transform_stmts_fold_state{accumulator.typed_statements.concat(mlc::Array{std::make_shared<semantic_ir::SemanticStatement>(semantic_ir::SemanticStatementContinue{source_span})}), accumulator.type_environment}; }
}, (*statement));
}
transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept{
  auto final_state = statements.fold(Transform_stmts_fold_state{{}, transform_context.type_env}, [=](Transform_stmts_fold_state accumulator, std::shared_ptr<ast::Stmt> statement) mutable { return transform_stmts_fold_step(accumulator, statement, transform_context); });
  return transform::TransformStmtsResult{final_state.typed_statements, final_state.type_environment};
}

} // namespace transform_stmts
