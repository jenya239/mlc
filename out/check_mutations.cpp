#define main mlc_user_main
#include "check_mutations.hpp"

namespace check_mutations {

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept{
return scope.any([=](mlc::String scope_entry) mutable { return (scope_entry == sought_name); });
}
mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept{
return std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [operation, left, right, source_span] = exprBin; return [&]() {
auto right_errors = check_mutation_expr(right, mutable_locals);
return ((operation == mlc::String("=", 1)) ? ([&]() {
auto assignment_errors = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, span] = exprIdent; return ((!scope_has(mutable_locals, name)) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error((mlc::String("cannot assign to immutable binding: ", 36) + name), span)}) : ([&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}())); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(); }
}, (*left));
return ast::diagnostics_append(assignment_errors, right_errors);
}()) : (ast::diagnostics_append(check_mutation_expr(left, mutable_locals), right_errors)));
}(); },
[&](const ast::ExprUn& exprUn) { auto [__0, inner, __2] = exprUn; return check_mutation_expr(inner, mutable_locals); },
[&](const ast::ExprCall& exprCall) { auto [function, arguments, __2] = exprCall; return check_mutation_call(function, arguments, mutable_locals); },
[&](const ast::ExprMethod& exprMethod) { auto [object, __1, arguments, __3] = exprMethod; return check_mutation_call(object, arguments, mutable_locals); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return check_mutation_expr(object, mutable_locals); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index, __2] = exprIndex; return ast::diagnostics_append(check_mutation_expr(object, mutable_locals), check_mutation_expr(index, mutable_locals)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_expr, else_expr, __3] = exprIf; return check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>>{condition, then_expr, else_expr}, mutable_locals); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result, __2] = exprBlock; return check_mutation_block(statements, result, mutable_locals); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, __2] = exprWhile; return ast::diagnostics_append(check_mutation_expr(condition, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterator, statements, __3] = exprFor; return ast::diagnostics_append(check_mutation_expr(iterator, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, __2] = exprMatch; return ast::diagnostics_append(check_mutation_expr(subject, mutable_locals), check_mutation_arms(arms, mutable_locals)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, lit_parts, __2] = exprRecord; return check_mutation_record_lit_parts(lit_parts, mutable_locals); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(check_mutation_expr(base, mutable_locals), check_mutation_field_vals(field_values, mutable_locals)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return check_mutation_exprs(elements, mutable_locals); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return check_mutation_exprs(elements, mutable_locals); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return check_mutation_expr(inner, mutable_locals); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return check_mutation_expr(body, mutable_locals); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner, __2] = exprNamedArg; return check_mutation_expr(inner, mutable_locals); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, stmts, __3] = exprWith; return ast::diagnostics_append(check_mutation_expr(resource, mutable_locals), check_mutation_stmts_errors(stmts, mutable_locals)); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> diagnostics_for_record_literal_single_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> mutable_locals) noexcept{
return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return check_mutation_field_vals(field_values, mutable_locals); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return check_mutation_expr(spread_expression, mutable_locals); }
}, literal_part);
}
mlc::Array<ast::Diagnostic> check_mutation_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> mutable_locals) noexcept{
return lit_parts.flat_map([=](ast::RecordLitPart literal_part_under_check) mutable { return diagnostics_for_record_literal_single_part(literal_part_under_check, mutable_locals); });
}
mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept{
auto errors_from_callable = check_mutation_expr(function, mutable_locals);
return arguments.fold(errors_from_callable, [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> argument_under_call) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(argument_under_call, mutable_locals)); });
}
mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept{
return expressions.fold([&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(), [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> expression_under_list) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression_under_list, mutable_locals)); });
}
mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept{
return field_values.fold([&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(), [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(field_value_under_walk->val, mutable_locals)); });
}
mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept{
return arms.fold([&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(), [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::MatchArm> arm_under_walk) mutable { return [&]() {
auto bindings = names::pattern_bindings(arm_under_walk->pat);
auto arm_scope = mutable_locals;
auto binding_index = 0;
while ((binding_index < bindings.length())) {
arm_scope.push_back(bindings[binding_index]);
binding_index = (binding_index + 1);
}
auto after_guard = (arm_under_walk->has_guard ? (ast::diagnostics_append(errors_accumulator, check_mutation_expr(arm_under_walk->when_condition, arm_scope))) : (errors_accumulator));
return ast::diagnostics_append(after_guard, check_mutation_expr(arm_under_walk->body, arm_scope));
}(); });
}
mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept{
auto inner_mutable = mutable_locals;
auto statement_errors = statements.fold([&]() {
auto starting_errors = mlc::Array<ast::Diagnostic>{};
return starting_errors;
}(), [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Stmt> statement_under_block) mutable { return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, is_mut, __2, value, __4] = stmtLet; return [&]() {
auto next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
if (is_mut) {
inner_mutable.push_back(name);
}
return next_errors;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, is_mut, __2, value, __4, __5, __6] = stmtLetPat; return [&]() {
auto next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
if (is_mut) {
[&]() {
auto bound_pattern_names = names::pattern_bindings(pattern);
auto pattern_binding_index = 0;
return [&]() {
while ((pattern_binding_index < bound_pattern_names.length())) {
inner_mutable.push_back(bound_pattern_names[pattern_binding_index]);
pattern_binding_index = (pattern_binding_index + 1);
}
}();
}();
}
return next_errors;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable)); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return errors_accumulator; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return errors_accumulator; }
}, (*statement_under_block)); });
return ast::diagnostics_append(statement_errors, check_mutation_expr(result, inner_mutable));
}
mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
auto inner_mutable = mutable_locals;
return statements.fold([&]() {
auto starting_errors = mlc::Array<ast::Diagnostic>{};
return starting_errors;
}(), [=](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, is_mut, __2, value, __4] = stmtLet; return [&]() {
auto next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
if (is_mut) {
inner_mutable.push_back(name);
}
return next_errors;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, is_mut, __2, value, __4, __5, __6] = stmtLetPat; return [&]() {
auto next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
if (is_mut) {
[&]() {
auto bound_pattern_names = names::pattern_bindings(pattern);
auto pattern_binding_index = 0;
return [&]() {
while ((pattern_binding_index < bound_pattern_names.length())) {
inner_mutable.push_back(bound_pattern_names[pattern_binding_index]);
pattern_binding_index = (pattern_binding_index + 1);
}
}();
}();
}
return next_errors;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable)); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return errors_accumulator; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return errors_accumulator; }
}, (*statement_under_walk)); });
}
mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept{
auto mutable_locals = params.fold([&]() {
auto empty_scope = mlc::Array<mlc::String>{};
return empty_scope;
}(), [=](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return [&]() {
if (parameter_under_check->is_mut) {
parameter_scope.push_back(parameter_under_check->name);
}
return parameter_scope;
}(); });
return check_mutation_expr(body, mutable_locals);
}

} // namespace check_mutations
