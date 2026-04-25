#include "check_mutations.hpp"

#include "ast.hpp"
#include "names.hpp"

namespace check_mutations {

using namespace ast;
using namespace names;
using namespace ast_tokens;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
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

mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, source_span] = _v_exprbin; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> right_errors = check_mutation_expr(right, mutable_locals);
  return operation == mlc::String("=") ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> assignment_errors = [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprIdent>((*left)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*left)._); auto [name, span] = _v_exprident; return !scope_has(mutable_locals, name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("cannot assign to immutable binding: ") + name, span)} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }();
  return ast::diagnostics_append(assignment_errors, right_errors);
 }() : ast::diagnostics_append(check_mutation_expr(left, mutable_locals), right_errors);
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner, _w1] = _v_exprun; return check_mutation_expr(inner, mutable_locals); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, arguments, _w0] = _v_exprcall; return check_mutation_call(function, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, _w0, arguments, _w1] = _v_exprmethod; return check_mutation_call(object, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return check_mutation_expr(object, mutable_locals); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index, _w0] = _v_exprindex; return ast::diagnostics_append(check_mutation_expr(object, mutable_locals), check_mutation_expr(index, mutable_locals)); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expr, else_expr, _w0] = _v_exprif; return check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>>{condition, then_expr, else_expr}, mutable_locals); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return check_mutation_block(statements, result, mutable_locals); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return ast::diagnostics_append(check_mutation_expr(condition, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [_w0, iterator, statements, _w1] = _v_exprfor; return ast::diagnostics_append(check_mutation_expr(iterator, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, _w0] = _v_exprmatch; return ast::diagnostics_append(check_mutation_expr(subject, mutable_locals), check_mutation_arms(arms, mutable_locals)); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, field_values, _w1] = _v_exprrecord; return check_mutation_field_vals(field_values, mutable_locals); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, field_values, _w1] = _v_exprrecordupdate; return ast::diagnostics_append(check_mutation_expr(base, mutable_locals), check_mutation_field_vals(field_values, mutable_locals)); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, _w0] = _v_exprquestion; return check_mutation_expr(inner, mutable_locals); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [_w0, body, _w1] = _v_exprlambda; return check_mutation_expr(body, mutable_locals); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }();}

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors = check_mutation_expr(function, mutable_locals);
int index = 0;
while (index < arguments.size()){
{
errors = ast::diagnostics_append(errors, check_mutation_expr(arguments[index], mutable_locals));
index = index + 1;
}
}
return errors;
}

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors = {};
int index = 0;
while (index < expressions.size()){
{
errors = ast::diagnostics_append(errors, check_mutation_expr(expressions[index], mutable_locals));
index = index + 1;
}
}
return errors;
}

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors = {};
int index = 0;
while (index < field_values.size()){
{
errors = ast::diagnostics_append(errors, check_mutation_expr(field_values[index]->val, mutable_locals));
index = index + 1;
}
}
return errors;
}

mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors = {};
int index = 0;
while (index < arms.size()){
{
errors = ast::diagnostics_append(errors, check_mutation_expr(arms[index]->body, mutable_locals));
index = index + 1;
}
}
return errors;
}

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<mlc::String> inner_mutable = mutable_locals;
mlc::Array<ast::Diagnostic> errors = {};
int index = 0;
while (index < statements.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
inner_mutable.push_back(name);
}
}
  return std::make_tuple();
 }(); },
  [&](const StmtLetPat& stmtletpat) -> std::tuple<> { auto [pattern, is_mut, _w0, value, _w1] = stmtletpat; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
mlc::Array<mlc::String> pnames = names::pattern_bindings(pattern);
int pi = 0;
[&]() { 
  while (pi < pnames.size()){
{
inner_mutable.push_back(pnames[pi]);
pi = pi + 1;
}
}
 }();
}
}
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [name, _w0, value, _w1] = stmtletconst; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, _w0] = stmtexpr; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [expression, _w0] = stmtreturn; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [_w0] = stmtbreak; return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [_w0] = stmtcontinue; return std::make_tuple(); }
}, (*statements[index])._);
index = index + 1;
}
}
return ast::diagnostics_append(errors, check_mutation_expr(result, inner_mutable));
}

mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<mlc::String> inner_mutable = mutable_locals;
mlc::Array<ast::Diagnostic> errors = {};
int index = 0;
while (index < statements.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> std::tuple<> { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
inner_mutable.push_back(name);
}
}
  return std::make_tuple();
 }(); },
  [&](const StmtLetPat& stmtletpat) -> std::tuple<> { auto [pattern, is_mut, _w0, value, _w1] = stmtletpat; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
mlc::Array<mlc::String> pnames = names::pattern_bindings(pattern);
int pi = 0;
[&]() { 
  while (pi < pnames.size()){
{
inner_mutable.push_back(pnames[pi]);
pi = pi + 1;
}
}
 }();
}
}
  return std::make_tuple();
 }(); },
  [&](const StmtLetConst& stmtletconst) -> std::tuple<> { auto [name, _w0, value, _w1] = stmtletconst; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(value, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) -> std::tuple<> { auto [expression, _w0] = stmtexpr; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) -> std::tuple<> { auto [expression, _w0] = stmtreturn; return [&]() -> std::tuple<> { 
  errors = ast::diagnostics_append(errors, check_mutation_expr(expression, inner_mutable));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) -> std::tuple<> { auto [_w0] = stmtbreak; return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) -> std::tuple<> { auto [_w0] = stmtcontinue; return std::make_tuple(); }
}, (*statements[index])._);
index = index + 1;
}
}
return errors;
}

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept{
mlc::Array<mlc::String> mutable_locals = {};
int index = 0;
while (index < params.size()){
{
if (params[index]->is_mut){
{
mutable_locals.push_back(params[index]->name);
}
}
index = index + 1;
}
}
return check_mutation_expr(body, mutable_locals);
}

} // namespace check_mutations
