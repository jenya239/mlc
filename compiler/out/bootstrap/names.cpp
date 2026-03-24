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
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [name] = patIdent; return [&]() {
accumulator.push_back(name);
return accumulator;
}(); },
[&](const ast::PatCtor& patCtor) { auto [__0, sub_patterns] = patCtor; return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const ast::PatRecord& patRecord) { auto [__0, field_pats] = patRecord; return [&]() {
auto index = 0;
while ((index < field_pats.length())) {
accumulator = collect_pattern_bindings(field_pats[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](const auto& __v) { return accumulator; }
}, (*pattern));
}
mlc::HashMap<mlc::String, bool> collect_globals(ast::Program prog) noexcept{
auto names = mlc::HashMap<mlc::String, bool>();
names.set(mlc::String("true", 4), true);
names.set(mlc::String("false", 5), true);
names.set(mlc::String("exit", 4), true);
names.set(mlc::String("print", 5), true);
names.set(mlc::String("println", 7), true);
names.set(mlc::String("args", 4), true);
names.set(mlc::String("File", 4), true);
names.set(mlc::String("Shared", 6), true);
names.set(mlc::String("Map", 3), true);
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.set(name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclType& declType) { auto [name, __1, variants] = declType; return [&]() {
names.set(name, true);
auto vi = 0;
while ((vi < variants.length())) {
std::visit(overloaded{[&](const ast::VarUnit& varUnit) { auto [variant_name] = varUnit; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarTuple& varTuple) { auto [variant_name, __1] = varTuple; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const ast::VarRecord& varRecord) { auto [variant_name, __1] = varRecord; return [&]() {
names.set(variant_name, true);
/* unit */;
return /* unit */;
}(); }
}, (*variants[vi]));
vi = (vi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclTrait& declTrait) { auto [name, __1, methods] = declTrait; return [&]() {
names.set(name, true);
auto mi = 0;
while ((mi < methods.length())) {
std::visit(overloaded{[&](const ast::DeclFn& declFn) { auto [fn_name, __1, __2, __3, __4, __5] = declFn; return [&]() {
names.set(fn_name, true);
/* unit */;
return /* unit */;
}(); },
[&](const auto& __v) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*methods[mi]));
mi = (mi + 1);
}
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExtend& declExtend) { auto [__0, __1, __2] = declExtend; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclImport& declImport) { auto [__0, __1] = declImport; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::DeclExported& declExported) { auto [__0] = declExported; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*ast::decl_inner(prog.decls[i])));
i = (i + 1);
}
return names;
}
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<ast::Expr> expr, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name] = exprIdent; return [&]() {
auto errors = mlc::Array<mlc::String>{};
if (((!scope_contains(locals, name)) && (!globals.has(name)))) {
errors.push_back((mlc::String("undefined: ", 11) + name));
}
return errors;
}(); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right] = exprBin; return ast::errs_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals)); },
[&](const ast::ExprUn& exprUn) { auto [__0, inner] = exprUn; return check_names_expr(inner, locals, globals); },
[&](const ast::ExprCall& exprCall) { auto [func, call_args] = exprCall; return [&]() {
auto errors = check_names_expr(func, locals, globals);
auto i = 0;
while ((i < call_args.length())) {
errors = ast::errs_append(errors, check_names_expr(call_args[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [obj, __1, margs] = exprMethod; return [&]() {
auto errors = check_names_expr(obj, locals, globals);
auto i = 0;
while ((i < margs.length())) {
errors = ast::errs_append(errors, check_names_expr(margs[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprField& exprField) { auto [obj, __1] = exprField; return check_names_expr(obj, locals, globals); },
[&](const ast::ExprIndex& exprIndex) { auto [obj, index_expr] = exprIndex; return ast::errs_append(check_names_expr(obj, locals, globals), check_names_expr(index_expr, locals, globals)); },
[&](const ast::ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; return ast::errs_append(ast::errs_append(check_names_expr(cond, locals, globals), check_names_expr(then_expr, locals, globals)), check_names_expr(else_expr, locals, globals)); },
[&](const ast::ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; return [&]() {
auto stmt_result = check_names_stmts(stmts, locals, globals);
return NameCheckResult_append_expr_errors(stmt_result, check_names_expr(result, stmt_result.scope, globals));
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; return [&]() {
auto check_result = check_names_stmts(stmts, locals, globals);
return ast::errs_append(check_names_expr(cond, locals, globals), check_result.errors);
}(); },
[&](const ast::ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; return [&]() {
auto inner = locals;
inner.push_back(var);
auto check_result = check_names_stmts(stmts, inner, globals);
return ast::errs_append(check_names_expr(iter, locals, globals), check_result.errors);
}(); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; return [&]() {
auto errors = check_names_expr(subject, locals, globals);
auto i = 0;
while ((i < arms.length())) {
auto bindings = pattern_bindings(arms[i]->pat);
auto arm_scope = locals;
auto index = 0;
while ((index < bindings.length())) {
arm_scope.push_back(bindings[index]);
index = (index + 1);
}
errors = ast::errs_append(errors, check_names_expr(arms[i]->body, arm_scope, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, field_vals] = exprRecord; return [&]() {
auto errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_vals.length())) {
errors = ast::errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_vals] = exprRecordUpdate; return [&]() {
auto errors = check_names_expr(base, locals, globals);
auto i = 0;
while ((i < field_vals.length())) {
errors = ast::errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprArray& exprArray) { auto [exprs] = exprArray; return [&]() {
auto errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < exprs.length())) {
errors = ast::errs_append(errors, check_names_expr(exprs[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; return check_names_expr(inner, locals, globals); },
[&](const ast::ExprLambda& exprLambda) { auto [params, body] = exprLambda; return [&]() {
auto lambda_locals = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < locals.length())) {
lambda_locals.push_back(locals[i]);
i = (i + 1);
}
auto pi = 0;
while ((pi < params.length())) {
lambda_locals.push_back(params[pi]);
pi = (pi + 1);
}
return check_names_expr(body, lambda_locals, globals);
}(); },
[&](const auto& __v) { return [&]() {
auto empty_errors = mlc::Array<mlc::String>{};
return empty_errors;
}(); }
}, (*expr));
}
mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept{
return ast::errs_append(self.errors, expr_errors);
}
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
auto errors = mlc::Array<mlc::String>{};
auto scope = locals;
auto i = 0;
while ((i < stmts.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, __2, value] = stmtLet; return [&]() {
errors = ast::errs_append(errors, check_names_expr(value, scope, globals));
scope.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expr] = stmtExpr; return [&]() {
errors = ast::errs_append(errors, check_names_expr(expr, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expr] = stmtReturn; return [&]() {
errors = ast::errs_append(errors, check_names_expr(expr, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtBreak& stmtBreak) { return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtContinue& stmtContinue) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*stmts[i]));
i = (i + 1);
}
return NameCheckResult{errors, scope};
}

} // namespace names
