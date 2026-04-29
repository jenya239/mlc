#define main mlc_user_main
#include "check_mutations.hpp"

namespace check_mutations {

bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
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
mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept{
return std::visit(overloaded{[&](const ast::ExprBin& exprBin) { auto [operation, left, right, source_span] = exprBin; return [&]() {
auto right_errors = check_mutation_expr(right, mutable_locals);
return ((operation == mlc::String("=", 1)) ? [&]() {
auto assignment_errors = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, span] = exprIdent; return ((!scope_has(mutable_locals, name)) ? mlc::Array{ast::diagnostic_error((mlc::String("cannot assign to immutable binding: ", 36) + name), span)} : [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}()); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(); }
}, (*left));
return ast::diagnostics_append(assignment_errors, right_errors);
}() : ast::diagnostics_append(check_mutation_expr(left, mutable_locals), right_errors));
}(); },
[&](const ast::ExprUn& exprUn) { auto [__0, inner, __2] = exprUn; return check_mutation_expr(inner, mutable_locals); },
[&](const ast::ExprCall& exprCall) { auto [function, arguments, __2] = exprCall; return check_mutation_call(function, arguments, mutable_locals); },
[&](const ast::ExprMethod& exprMethod) { auto [object, __1, arguments, __3] = exprMethod; return check_mutation_call(object, arguments, mutable_locals); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return check_mutation_expr(object, mutable_locals); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index, __2] = exprIndex; return ast::diagnostics_append(check_mutation_expr(object, mutable_locals), check_mutation_expr(index, mutable_locals)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_expr, else_expr, __3] = exprIf; return check_mutation_exprs(mlc::Array{condition, then_expr, else_expr}, mutable_locals); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result, __2] = exprBlock; return check_mutation_block(statements, result, mutable_locals); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, __2] = exprWhile; return ast::diagnostics_append(check_mutation_expr(condition, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterator, statements, __3] = exprFor; return ast::diagnostics_append(check_mutation_expr(iterator, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, __2] = exprMatch; return ast::diagnostics_append(check_mutation_expr(subject, mutable_locals), check_mutation_arms(arms, mutable_locals)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, field_values, __2] = exprRecord; return check_mutation_field_vals(field_values, mutable_locals); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(check_mutation_expr(base, mutable_locals), check_mutation_field_vals(field_values, mutable_locals)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return check_mutation_exprs(elements, mutable_locals); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return check_mutation_exprs(elements, mutable_locals); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return check_mutation_expr(inner, mutable_locals); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return check_mutation_expr(body, mutable_locals); },
[&](const auto& __v) { return [&]() {
auto empty = mlc::Array<ast::Diagnostic>{};
return empty;
}(); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept{
auto errors = check_mutation_expr(function, mutable_locals);
auto index = 0;
while ((index < arguments.length())) {
errors = ast::diagnostics_append(errors, check_mutation_expr(arguments[index], mutable_locals));
index = (index + 1);
}
return errors;
}
mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept{
auto errors = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < expressions.length())) {
errors = ast::diagnostics_append(errors, check_mutation_expr(expressions[index], mutable_locals));
index = (index + 1);
}
return errors;
}
mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept{
auto errors = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < field_values.length())) {
errors = ast::diagnostics_append(errors, check_mutation_expr(field_values[index]->val, mutable_locals));
index = (index + 1);
}
return errors;
}
mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept{
auto errors = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < arms.length())) {
errors = ast::diagnostics_append(errors, check_mutation_expr(arms[index]->body, mutable_locals));
index = (index + 1);
}
return errors;
}
mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept{
auto inner_mutable = mutable_locals;
auto errors = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < statements.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, is_mut, __2, value, __4] = stmtLet; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
if (is_mut) {
inner_mutable.push_back(name);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, is_mut, __2, value, __4] = stmtLetPat; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
if (is_mut) {
[&]() {
auto pnames = names::pattern_bindings(pattern);
auto pi = 0;
return [&]() {
while ((pi < pnames.length())) {
inner_mutable.push_back(pnames[pi]);
pi = (pi + 1);
}
}();
}();
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, __1, value, __3] = stmtLetConst; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
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
return ast::diagnostics_append(errors, check_mutation_expr(result, inner_mutable));
}
mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
auto inner_mutable = mutable_locals;
auto errors = mlc::Array<ast::Diagnostic>{};
auto index = 0;
while ((index < statements.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, is_mut, __2, value, __4] = stmtLet; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
if (is_mut) {
inner_mutable.push_back(name);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, is_mut, __2, value, __4] = stmtLetPat; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
if (is_mut) {
[&]() {
auto pnames = names::pattern_bindings(pattern);
auto pi = 0;
return [&]() {
while ((pi < pnames.length())) {
inner_mutable.push_back(pnames[pi]);
pi = (pi + 1);
}
}();
}();
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, __1, value, __3] = stmtLetConst; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return [&]() {
errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
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
return errors;
}
mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept{
auto mutable_locals = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < params.length())) {
if (params[index]->is_mut) {
mutable_locals.push_back(params[index]->name);
}
index = (index + 1);
}
return check_mutation_expr(body, mutable_locals);
}

} // namespace check_mutations
