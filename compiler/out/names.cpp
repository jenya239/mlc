#include "names.hpp"

#include "ast.hpp"

namespace names {

using namespace ast;
using namespace ast_tokens;

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept;

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program prog) noexcept;

mlc::Array<mlc::String> check_names_expr(std::shared_ptr<ast::Expr> expr, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

mlc::Array<mlc::String> NameCheckResult_append_expr_errors(names::NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept;

names::NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept;

bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
bool found = false;
int index = 0;
while (index < scope.size() && !found){
{
if (scope[index] == name){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}

mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<ast::Pat> pattern) noexcept{
mlc::Array<mlc::String> accumulator = {};
return collect_pattern_bindings(pattern, accumulator);
}

mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<ast::Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::PatIdent>((*pattern))) { auto _v_patident = std::get<ast::PatIdent>((*pattern)); auto [name] = _v_patident; return [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(name);
  return accumulator;
 }(); } if (std::holds_alternative<ast::PatCtor>((*pattern))) { auto _v_patctor = std::get<ast::PatCtor>((*pattern)); auto [_w0, sub_patterns] = _v_patctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } return accumulator; }();
}

mlc::HashMap<mlc::String, bool> collect_globals(ast::Program prog) noexcept{
mlc::HashMap<mlc::String, bool> names = mlc::HashMap<mlc::String, bool>();
names.set(mlc::String("true"), true);
names.set(mlc::String("false"), true);
names.set(mlc::String("exit"), true);
names.set(mlc::String("print"), true);
names.set(mlc::String("println"), true);
names.set(mlc::String("args"), true);
names.set(mlc::String("File"), true);
names.set(mlc::String("Shared"), true);
names.set(mlc::String("Map"), true);
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return [&]() -> std::tuple<> { 
  names.set(name, true);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) { auto [name, variants] = decltype_; return [&]() -> std::tuple<> { 
  names.set(name, true);
  int j = 0;
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) { auto [variant_name, _w0] = vartuple; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); },
  [&](const VarRecord& varrecord) { auto [variant_name, _w0] = varrecord; return [&]() -> std::tuple<> { 
  names.set(variant_name, true);
  return std::make_tuple();
 }(); }
}, (*variants[j]));
j = j + 1;
}
}
  return std::make_tuple();
 }(); },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) { auto [_w0] = declexported; return std::make_tuple(); }
}, (*ast::decl_inner(prog.decls[i])));
i = i + 1;
}
}
return names;
}

mlc::Array<mlc::String> check_names_expr(std::shared_ptr<ast::Expr> expr, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ast::ExprIdent>((*expr)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*expr)._); auto [name] = _v_exprident; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  if (!scope_contains(locals, name) && !globals.has(name)){
{
errors.push_back(mlc::String("undefined: ") + name);
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprBin>((*expr)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expr)._); auto [_w0, left, right] = _v_exprbin; return ast::errs_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals)); } if (std::holds_alternative<ast::ExprUn>((*expr)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expr)._); auto [_w0, inner] = _v_exprun; return check_names_expr(inner, locals, globals); } if (std::holds_alternative<ast::ExprCall>((*expr)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expr)._); auto [func, call_args] = _v_exprcall; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(func, locals, globals);
  int i = 0;
  while (i < call_args.size()){
{
errors = ast::errs_append(errors, check_names_expr(call_args[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprMethod>((*expr)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expr)._); auto [obj, _w0, margs] = _v_exprmethod; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(obj, locals, globals);
  int i = 0;
  while (i < margs.size()){
{
errors = ast::errs_append(errors, check_names_expr(margs[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprField>((*expr)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expr)._); auto [obj, _w0] = _v_exprfield; return check_names_expr(obj, locals, globals); } if (std::holds_alternative<ast::ExprIndex>((*expr)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expr)._); auto [obj, idx] = _v_exprindex; return ast::errs_append(check_names_expr(obj, locals, globals), check_names_expr(idx, locals, globals)); } if (std::holds_alternative<ast::ExprIf>((*expr)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expr)._); auto [cond, then_expr, else_expr] = _v_exprif; return ast::errs_append(ast::errs_append(check_names_expr(cond, locals, globals), check_names_expr(then_expr, locals, globals)), check_names_expr(else_expr, locals, globals)); } if (std::holds_alternative<ast::ExprBlock>((*expr)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expr)._); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::Array<mlc::String> { 
  names::NameCheckResult stmt_result = check_names_stmts(stmts, locals, globals);
  return NameCheckResult_append_expr_errors(stmt_result, check_names_expr(result, stmt_result.scope, globals));
 }(); } if (std::holds_alternative<ast::ExprWhile>((*expr)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expr)._); auto [cond, stmts] = _v_exprwhile; return [&]() -> mlc::Array<mlc::String> { 
  names::NameCheckResult check_result = check_names_stmts(stmts, locals, globals);
  return ast::errs_append(check_names_expr(cond, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ast::ExprFor>((*expr)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expr)._); auto [var, iter, stmts] = _v_exprfor; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> inner = locals;
  inner.push_back(var);
  names::NameCheckResult check_result = check_names_stmts(stmts, inner, globals);
  return ast::errs_append(check_names_expr(iter, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ast::ExprMatch>((*expr)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expr)._); auto [subject, arms] = _v_exprmatch; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(subject, locals, globals);
  int i = 0;
  while (i < arms.size()){
{
mlc::Array<mlc::String> bindings = pattern_bindings(arms[i]->pat);
mlc::Array<mlc::String> arm_scope = locals;
int index = 0;
while (index < bindings.size()){
{
arm_scope.push_back(bindings[index]);
index = index + 1;
}
}
errors = ast::errs_append(errors, check_names_expr(arms[i]->body, arm_scope, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprRecord>((*expr)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expr)._); auto [_w0, field_vals] = _v_exprrecord; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < field_vals.size()){
{
errors = ast::errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expr)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expr)._); auto [_w0, base, field_vals] = _v_exprrecordupdate; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(base, locals, globals);
  int i = 0;
  while (i < field_vals.size()){
{
errors = ast::errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprArray>((*expr)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expr)._); auto [exprs] = _v_exprarray; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < exprs.size()){
{
errors = ast::errs_append(errors, check_names_expr(exprs[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ast::ExprQuestion>((*expr)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expr)._); auto [inner] = _v_exprquestion; return check_names_expr(inner, locals, globals); } if (std::holds_alternative<ast::ExprLambda>((*expr)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expr)._); auto [params, body] = _v_exprlambda; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> lambda_locals = {};
  int i = 0;
  while (i < locals.size()){
{
lambda_locals.push_back(locals[i]);
i = i + 1;
}
}
  int j = 0;
  while (j < params.size()){
{
lambda_locals.push_back(params[j]);
j = j + 1;
}
}
  return check_names_expr(body, lambda_locals, globals);
 }(); } return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> e = {};
  return e;
 }(); }();
}

mlc::Array<mlc::String> NameCheckResult_append_expr_errors(names::NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept{return ast::errs_append(self.errors, expr_errors);}

names::NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::HashMap<mlc::String, bool> globals) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::Array<mlc::String> scope = locals;
int i = 0;
while (i < stmts.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, _w1, value] = stmtlet; return [&]() -> std::tuple<> { 
  errors = ast::errs_append(errors, check_names_expr(value, scope, globals));
  scope.push_back(name);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return [&]() -> std::tuple<> { 
  errors = ast::errs_append(errors, check_names_expr(expr, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return [&]() -> std::tuple<> { 
  errors = ast::errs_append(errors, check_names_expr(expr, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) { return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) { return std::make_tuple(); }
}, (*stmts[i])._);
i = i + 1;
}
}
return names::NameCheckResult{errors, scope};
}

} // namespace names
