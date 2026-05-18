#define main mlc_user_main
#include "names.hpp"

namespace names {

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
return scope.any([=](mlc::String scope_entry) mutable { return (scope_entry == name); });
}
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept{
auto accumulator = mlc::Array<mlc::String>{};
return collect_pattern_bindings(pattern, accumulator);
}
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept{
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name, __1] = patIdent; return [&]() {
accumulator.push_back(name);
return accumulator;
}(); },
[&](const ast::PatCtor& patCtor) { auto [__0, sub_patterns, __2] = patCtor; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatRecord& patRecord) { auto [__0, field_patterns, __2] = patRecord; return [&]() {
auto index = 0;
while ((index < field_patterns.length())) {
accumulator = collect_pattern_bindings(field_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatTuple& patTuple) { auto [sub_patterns, __1] = patTuple; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatArray& patArray) { auto [subs, rest, __2] = patArray; return [&]() {
auto index = 0;
while ((index < subs.length())) {
accumulator = collect_pattern_bindings(subs[index], accumulator);
index = (index + 1);
}
return (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1))) ? ([&]() {
accumulator.push_back(rest);
return accumulator;
}()) : (accumulator));
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? (collect_pattern_bindings(alts[0], accumulator)) : (accumulator)); },
[&](const auto& __v) { return accumulator; }
}, (*pattern));
}
mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{
if (((!scope_contains(locals, name)) && (!globals.has(name)))) {
return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(((mlc::String("undefined: ", 11) + mlc::to_string(name)) + mlc::String("", 0)), source_span)};
} else {
auto empty_ident_diagnostics = mlc::Array<ast::Diagnostic>{};
return empty_ident_diagnostics;
}
}
mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return ast::diagnostics_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals));
}
mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return check_names_expr(inner, locals, globals);
}
mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto diagnostics_from_callable = check_names_expr(function, locals, globals);
return call_arguments.fold(diagnostics_from_callable, [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> argument_under_call) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(argument_under_call, locals, globals)); });
}
mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto diagnostics_from_receiver = check_names_expr(object, locals, globals);
return method_arguments.fold(diagnostics_from_receiver, [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> argument_under_method_call) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(argument_under_method_call, locals, globals)); });
}
mlc::Array<ast::Diagnostic> check_names_field_expression(std::shared_ptr<ast::Expr> object, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return check_names_expr(object, locals, globals);
}
mlc::Array<ast::Diagnostic> check_names_index_expression(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return ast::diagnostics_append(check_names_expr(object, locals, globals), check_names_expr(index_expression, locals, globals));
}
mlc::Array<ast::Diagnostic> check_names_conditional_expression(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return ast::diagnostics_append(ast::diagnostics_append(check_names_expr(condition, locals, globals), check_names_expr(then_expression, locals, globals)), check_names_expr(else_expression, locals, globals));
}
mlc::Array<ast::Diagnostic> check_names_block_expression(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto statement_result = check_names_statements(statements, locals, globals);
return NameCheckResult_append_expression_diagnostics(statement_result, check_names_expr(result, statement_result.scope, globals));
}
mlc::Array<ast::Diagnostic> check_names_while_expression(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto statement_check = check_names_statements(statements, locals, globals);
return ast::diagnostics_append(check_names_expr(condition, locals, globals), statement_check.diagnostics);
}
mlc::Array<ast::Diagnostic> check_names_for_expression(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto inner_scope = locals;
inner_scope.push_back(variable_name);
auto statement_check = check_names_statements(statements, inner_scope, globals);
return ast::diagnostics_append(check_names_expr(iterator, locals, globals), statement_check.diagnostics);
}
mlc::Array<ast::Diagnostic> check_names_with_expression(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto inner_scope = locals;
inner_scope.push_back(binder);
auto stmts_check = check_names_statements(stmts, inner_scope, globals);
return ast::diagnostics_append(check_names_expr(resource, locals, globals), stmts_check.diagnostics);
}
mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto diagnostics_after_subject = check_names_expr(subject, locals, globals);
return arms.fold(diagnostics_after_subject, [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::MatchArm> arm_under_walk) mutable { return [&]() {
auto bindings = pattern_bindings(arm_under_walk->pat);
auto arm_scope = locals;
auto binding_index = 0;
while ((binding_index < bindings.length())) {
arm_scope.push_back(bindings[binding_index]);
binding_index = (binding_index + 1);
}
auto after_guard = (arm_under_walk->has_guard ? (ast::diagnostics_append(diagnostics_accumulator, check_names_expr(arm_under_walk->when_condition, arm_scope, globals))) : (diagnostics_accumulator));
return ast::diagnostics_append(after_guard, check_names_expr(arm_under_walk->body, arm_scope, globals));
}(); });
}
mlc::Array<ast::Diagnostic> diagnostics_for_single_record_literal_name_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return check_names_record_expression(field_values, locals, globals); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return check_names_expr(spread_expression, locals, globals); }
}, literal_part);
}
mlc::Array<ast::Diagnostic> check_names_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return lit_parts.flat_map([=](ast::RecordLitPart literal_part_under_walk) mutable { return diagnostics_for_single_record_literal_name_part(literal_part_under_walk, locals, globals); });
}
mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return field_values.fold([&]() {
auto starting_record_names = mlc::Array<ast::Diagnostic>{};
return starting_record_names;
}(), [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(field_value_under_walk->val, locals, globals)); });
}
mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return field_values.fold(check_names_expr(base, locals, globals), [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(field_value_under_walk->val, locals, globals)); });
}
mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return elements.fold([&]() {
auto starting_array_names = mlc::Array<ast::Diagnostic>{};
return starting_array_names;
}(), [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Expr> element_under_walk) mutable { return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(element_under_walk, locals, globals)); });
}
mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return check_names_expr(inner, locals, globals);
}
mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto lambda_environment_names = parameter_names.fold(locals.fold([&]() {
auto enclosing_copied_into_lambda = mlc::Array<mlc::String>{};
return enclosing_copied_into_lambda;
}(), [=](mlc::Array<mlc::String> accumulated_enclosing_names, mlc::String enclosing_local_name) mutable { return [&]() {
accumulated_enclosing_names.push_back(enclosing_local_name);
return accumulated_enclosing_names;
}(); }), [=](mlc::Array<mlc::String> accumulated_scope_names, mlc::String lambda_parameter_name) mutable { return [&]() {
accumulated_scope_names.push_back(lambda_parameter_name);
return accumulated_scope_names;
}(); });
return check_names_expr(body, lambda_environment_names, globals);
}
mlc::Array<ast::Diagnostic> check_names_empty() noexcept{
auto empty_list = mlc::Array<ast::Diagnostic>{};
return empty_list;
}
mlc::Array<ast::Diagnostic> check_names_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, source_span] = exprIdent; return check_names_identifier(name, locals, globals, source_span); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return check_names_binary_expression(left, right, locals, globals); },
[&](const ast::ExprUn& exprUn) { auto [__0, inner, __2] = exprUn; return check_names_unary_expression(inner, locals, globals); },
[&](const ast::ExprCall& exprCall) { auto [function, call_arguments, __2] = exprCall; return check_names_call_expression(function, call_arguments, locals, globals); },
[&](const ast::ExprMethod& exprMethod) { auto [object, __1, method_arguments, __3] = exprMethod; return check_names_method_expression(object, method_arguments, locals, globals); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return check_names_field_expression(object, locals, globals); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return check_names_index_expression(object, index_expression, locals, globals); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_expression, else_expression, __3] = exprIf; return check_names_conditional_expression(condition, then_expression, else_expression, locals, globals); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result, __2] = exprBlock; return check_names_block_expression(statements, result, locals, globals); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, __2] = exprWhile; return check_names_while_expression(condition, statements, locals, globals); },
[&](const ast::ExprFor& exprFor) { auto [variable, iterator, statements, __3] = exprFor; return check_names_for_expression(variable, iterator, statements, locals, globals); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, __2] = exprMatch; return check_names_match_expression(subject, arms, locals, globals); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, lit_parts, __2] = exprRecord; return check_names_record_lit_parts(lit_parts, locals, globals); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return check_names_record_update_expression(base, field_values, locals, globals); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return check_names_array_expression(elements, locals, globals); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return check_names_array_expression(elements, locals, globals); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return check_names_question_expression(inner, locals, globals); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return check_names_lambda_expression(parameter_names, body, locals, globals); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner, __2] = exprNamedArg; return check_names_expr(inner, locals, globals); },
[&](const ast::ExprWith& exprWith) { auto [resource, binder, stmts, __3] = exprWith; return check_names_with_expression(resource, binder, stmts, locals, globals); },
[&](const auto& __v) { return check_names_empty(); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{
return ast::diagnostics_append(self.diagnostics, expression_diagnostics);
}
NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto scope = locals;
auto collected_diagnostics = statements.fold([&]() {
auto starting_statement_diagnostics = mlc::Array<ast::Diagnostic>{};
return starting_statement_diagnostics;
}(), [=](mlc::Array<ast::Diagnostic> diagnostics_accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, __2, value, __4] = stmtLet; return [&]() {
auto next_after_let_value = ast::diagnostics_append(diagnostics_accumulator, check_names_expr(value, scope, globals));
scope.push_back(name);
return next_after_let_value;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, __1, __2, value, has_else, else_body, __6] = stmtLetPat; return [&]() {
auto next_after_pattern_let_value = ast::diagnostics_append(diagnostics_accumulator, check_names_expr(value, scope, globals));
auto bound_pattern_names = pattern_bindings(pattern);
auto pattern_binding_index = 0;
while ((pattern_binding_index < bound_pattern_names.length())) {
scope.push_back(bound_pattern_names[pattern_binding_index]);
pattern_binding_index = (pattern_binding_index + 1);
}
if (has_else) {
next_after_pattern_let_value = ast::diagnostics_append(next_after_pattern_let_value, check_names_expr(else_body, scope, globals));
}
return next_after_pattern_let_value;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, __1, value, __3] = stmtLetConst; return [&]() {
auto next_after_const = ast::diagnostics_append(diagnostics_accumulator, check_names_expr(value, scope, globals));
scope.push_back(name);
return next_after_const;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(expression, scope, globals)); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return ast::diagnostics_append(diagnostics_accumulator, check_names_expr(expression, scope, globals)); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return diagnostics_accumulator; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return diagnostics_accumulator; }
}, (*statement_under_walk)); });
return NameCheckResult{collected_diagnostics, scope};
}

} // namespace names
