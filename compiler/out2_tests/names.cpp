#define main mlc_user_main
#include "names.hpp"

namespace names {

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
auto found = false;
auto index = 0;
while (((index < scope.length()) && (!found))) {
if ((scope[index] == name)) {
found = true;
}
index = (index + 1);
}
return found;
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
return (((rest != mlc::String("", 0)) && (rest != mlc::String("_", 1))) ? [&]() {
accumulator.push_back(rest);
return accumulator;
}() : accumulator);
}(); },
[&](const ast::PatOr& patOr) { auto [alts, __1] = patOr; return ((alts.length() > 0) ? collect_pattern_bindings(alts[0], accumulator) : accumulator); },
[&](const auto& __v) { return accumulator; }
}, (*pattern));
}
mlc::Array<ast::Diagnostic> check_names_identifier(mlc::String name, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals, ast::Span source_span) noexcept{
auto collected = mlc::Array<ast::Diagnostic>{};
if (((!scope_contains(locals, name)) && (!globals.has(name)))) {
collected.push_back(ast::diagnostic_error((mlc::String("undefined: ", 11) + name), source_span));
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_binary_expression(std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return ast::diagnostics_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals));
}
mlc::Array<ast::Diagnostic> check_names_unary_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return check_names_expr(inner, locals, globals);
}
mlc::Array<ast::Diagnostic> check_names_call_expression(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = check_names_expr(function, locals, globals);
auto index = 0;
while ((index < call_arguments.length())) {
collected = ast::diagnostics_append(collected, check_names_expr(call_arguments[index], locals, globals));
index = (index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_method_expression(std::shared_ptr<ast::Expr> object, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = check_names_expr(object, locals, globals);
auto index = 0;
while ((index < method_arguments.length())) {
collected = ast::diagnostics_append(collected, check_names_expr(method_arguments[index], locals, globals));
index = (index + 1);
}
return collected;
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
mlc::Array<ast::Diagnostic> check_names_match_expression(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = check_names_expr(subject, locals, globals);
auto arm_index = 0;
while ((arm_index < arms.length())) {
auto bindings = pattern_bindings(arms[arm_index]->pat);
auto arm_scope = locals;
auto binding_index = 0;
while ((binding_index < bindings.length())) {
arm_scope.push_back(bindings[binding_index]);
binding_index = (binding_index + 1);
}
collected = ast::diagnostics_append(collected, check_names_expr(arms[arm_index]->body, arm_scope, globals));
arm_index = (arm_index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_record_expression(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < field_values.length())) {
collected = ast::diagnostics_append(collected, check_names_expr(field_values[index]->val, locals, globals));
index = (index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_record_update_expression(std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = check_names_expr(base, locals, globals);
auto index = 0;
while ((index < field_values.length())) {
collected = ast::diagnostics_append(collected, check_names_expr(field_values[index]->val, locals, globals));
index = (index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_array_expression(mlc::Array<std::shared_ptr<ast::Expr>> elements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < elements.length())) {
collected = ast::diagnostics_append(collected, check_names_expr(elements[index], locals, globals));
index = (index + 1);
}
return collected;
}
mlc::Array<ast::Diagnostic> check_names_question_expression(std::shared_ptr<ast::Expr> inner, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return check_names_expr(inner, locals, globals);
}
mlc::Array<ast::Diagnostic> check_names_lambda_expression(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto lambda_locals = mlc::Array<mlc::String>{};
auto local_index = 0;
while ((local_index < locals.length())) {
lambda_locals.push_back(locals[local_index]);
local_index = (local_index + 1);
}
auto parameter_index = 0;
while ((parameter_index < parameter_names.length())) {
lambda_locals.push_back(parameter_names[parameter_index]);
parameter_index = (parameter_index + 1);
}
return check_names_expr(body, lambda_locals, globals);
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
[&](const ast::ExprRecord& exprRecord) { auto [__0, field_values, __2] = exprRecord; return check_names_record_expression(field_values, locals, globals); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return check_names_record_update_expression(base, field_values, locals, globals); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return check_names_array_expression(elements, locals, globals); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return check_names_array_expression(elements, locals, globals); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return check_names_question_expression(inner, locals, globals); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return check_names_lambda_expression(parameter_names, body, locals, globals); },
[&](const auto& __v) { return check_names_empty(); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> NameCheckResult_append_expression_diagnostics(NameCheckResult self, mlc::Array<ast::Diagnostic> expression_diagnostics) noexcept{
return ast::diagnostics_append(self.diagnostics, expression_diagnostics);
}
NameCheckResult check_names_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto collected_diagnostics = mlc::Array<ast::Diagnostic>{};
auto scope = locals;
auto index = 0;
while ((index < statements.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, __2, value, __4] = stmtLet; return [&]() {
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
scope.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, __1, __2, value, __4] = stmtLetPat; return [&]() {
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
auto pnames = pattern_bindings(pattern);
auto j = 0;
while ((j < pnames.length())) {
scope.push_back(pnames[j]);
j = (j + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, __1, value, __3] = stmtLetConst; return [&]() {
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(value, scope, globals));
scope.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return [&]() {
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(expression, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return [&]() {
collected_diagnostics = ast::diagnostics_append(collected_diagnostics, check_names_expr(expression, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*statements[index]));
index = (index + 1);
}
return NameCheckResult{collected_diagnostics, scope};
}

} // namespace names
