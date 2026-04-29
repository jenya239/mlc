#define main mlc_user_main
#include "transform_stmts.hpp"

namespace transform_stmts {

transform::TransformStmtsResult transform_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext transform_context) noexcept{
auto typed_statements = mlc::Array<std::shared_ptr<semantic_ir::SStmt>>{};
auto current_env = transform_context.type_env;
auto statement_index = 0;
while ((statement_index < statements.length())) {
auto current_context = transform::transform_context_with_env(transform_context, current_env);
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [binding_name, is_mut, annotation, value_expression, source_span] = stmtLet; return [&]() {
auto annotated_type = registry::type_from_annotation(annotation);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = std::visit(overloaded{[&](const registry::TUnit& tUnit) { return inferred_type; },
[&](const auto& __v) { return annotated_type; }
}, (*annotated_type));
current_env.set(binding_name, value_type);
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLet{binding_name, is_mut, coerced_value, value_type, source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, is_mut, annotation, value_expression, source_span] = stmtLetPat; return [&]() {
auto annotated_type = registry::type_from_annotation(annotation);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = std::visit(overloaded{[&](const registry::TUnit& tUnit) { return inferred_type; },
[&](const auto& __v) { return annotated_type; }
}, (*annotated_type));
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
let_pattern_infer::infer_let_pattern_env(pattern, value_type, current_env, current_context.registry);
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetPat{pattern, is_mut, coerced_value, value_type, source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [binding_name, annotation, value_expression, source_span] = stmtLetConst; return [&]() {
auto annotated_type = registry::type_from_annotation(annotation);
auto typed_value = transform::transform_expr(value_expression, current_context, transform_stmts);
auto inferred_type = semantic_ir::sexpr_type(typed_value);
auto value_type = std::visit(overloaded{[&](const registry::TUnit& tUnit) { return inferred_type; },
[&](const auto& __v) { return annotated_type; }
}, (*annotated_type));
current_env.set(binding_name, value_type);
auto coerced_value = transform::coerce_expr_to_type(typed_value, value_type);
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtLetConst{binding_name, coerced_value, value_type, source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, source_span] = stmtExpr; return [&]() {
auto typed_expr = transform::transform_expr(expression, current_context, transform_stmts);
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtExpr{typed_expr, source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [return_expression, source_span] = stmtReturn; return [&]() {
auto typed_return = transform::transform_expr(return_expression, current_context, transform_stmts);
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtReturn{typed_return, source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtBreak& stmtBreak) { auto [source_span] = stmtBreak; return [&]() {
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtBreak{source_span}));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtContinue& stmtContinue) { auto [source_span] = stmtContinue; return [&]() {
typed_statements.push_back(std::make_shared<semantic_ir::SStmt>(semantic_ir::SStmtContinue{source_span}));
/* unit */;
return /* unit */;
}(); }
}, (*statements[statement_index]));
statement_index = (statement_index + 1);
}
return transform::TransformStmtsResult{typed_statements, current_env};
}

} // namespace transform_stmts
