#define main mlc_user_main
#include "escape_analysis.hpp"

#include "ast.hpp"

namespace escape_analysis {

using namespace ast;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_string_list() noexcept{
  auto empty = mlc::Array<mlc::String>{};
  return empty;
}
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept{
  auto index = 0;
  while ((index < haystack.length()))   {
    if ((haystack[index] == needle))     {
      return true;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return false;
}
mlc::Array<mlc::String> string_list_add(mlc::Array<mlc::String> names, mlc::String name) noexcept{
  if ((name.length() == 0))   {
    return names;
  } else if (string_list_contains(names, name))   {
    return names;
  } else   {
    return names.concat(mlc::Array<mlc::String>{name});
  }
}
mlc::Array<mlc::String> string_list_difference(mlc::Array<mlc::String> left, mlc::Array<mlc::String> right) noexcept{
  auto result = empty_string_list();
  auto index = 0;
  while ((index < left.length()))   {
    if ((!string_list_contains(right, left[index])))     {
      (result = string_list_add(result, left[index]));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> mark_all_watched(mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < watched.length()))   {
    (result = string_list_add(result, watched[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
bool type_expr_is_function(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyFn>((*__match_subject))) {
const ast::TyFn& tyFn = std::get<ast::TyFn>((*__match_subject));
auto [__0, __1] = tyFn; return true;
}
return false;
std::abort();
}();
}
mlc::Array<mlc::String> function_typed_param_names(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  auto names = empty_string_list();
  auto index = 0;
  while ((index < parameters.length()))   {
    if (type_expr_is_function(ast::param_type_value(parameters[index])))     {
      (names = string_list_add(names, ast::param_name(parameters[index])));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return names;
}
mlc::Array<mlc::String> visit_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < field_values.length()))   {
    (result = visit_expr(field_values[index]->value, watched, result));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> visit_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < parts.length()))   {
    (result = std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return visit_field_values(field_values, watched, result); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return visit_expr(spread_expression, watched, result); }
}, parts[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> visit_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < arms.length()))   {
    auto arm = arms[index];
    if (arm->has_guard)     {
      (result = visit_expr(arm->when_condition, watched, result));
    }
    (result = visit_expr(arm->body, watched, result));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> visit_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < expressions.length()))   {
    (result = visit_expr(expressions[index], watched, result));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> visit_stmt_list(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = escaping;
  auto index = 0;
  while ((index < statements.length()))   {
    (result = visit_stmt(statements[index], watched, result));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> visit_call_like(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  auto result = [&]() -> mlc::Array<mlc::String> {
auto __match_subject = callee;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (string_list_contains(watched, name) ? (escaping) : (visit_expr(callee, watched, escaping)));
}
return visit_expr(callee, watched, escaping);
std::abort();
}();
  return visit_expr_list(arguments, watched, result);
}
mlc::Array<mlc::String> visit_stmt(std::shared_ptr<ast::Stmt> statement, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return visit_expr(value, watched, escaping); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return visit_expr(else_expression, watched, visit_expr(value, watched, escaping)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return visit_expr(value, watched, escaping); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return visit_expr(expression, watched, escaping); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return escaping; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return escaping; },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return visit_expr(expression, watched, escaping); }
}, (*statement));
}
mlc::Array<mlc::String> visit_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> watched, mlc::Array<mlc::String> escaping) noexcept{
  return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return (string_list_contains(watched, name) ? (string_list_add(escaping, name)) : (escaping)); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return escaping; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return escaping; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return escaping; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return escaping; },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return escaping; },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return escaping; },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return escaping; },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return escaping; },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return escaping; },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return escaping; },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return visit_expr(right, watched, visit_expr(left, watched, escaping)); },
[&](const ast::ExprUn& exprUn) { auto [__0, operand, __2] = exprUn; return visit_expr(operand, watched, escaping); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return visit_call_like(callee, arguments, watched, escaping); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return visit_expr_list(arguments, watched, visit_expr(receiver, watched, escaping)); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return visit_expr(object, watched, escaping); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return visit_expr(index_expression, watched, visit_expr(object, watched, escaping)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return visit_expr(else_branch, watched, visit_expr(then_branch, watched, visit_expr(condition, watched, escaping))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return visit_expr(result_expression, watched, visit_stmt_list(statements, watched, escaping)); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return visit_stmt_list(body, watched, visit_expr(condition, watched, escaping)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterable, body, __3] = exprFor; return visit_stmt_list(body, watched, visit_expr(iterable, watched, escaping)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return visit_match_arms(arms, watched, visit_expr(scrutinee, watched, escaping)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return visit_record_parts(parts, watched, escaping); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return visit_field_values(field_values, watched, visit_expr(base, watched, escaping)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return visit_expr_list(elements, watched, escaping); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return visit_expr_list(elements, watched, escaping); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return visit_expr(inner, watched, escaping); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return visit_expr(body, watched, escaping); },
[&](const ast::ExprSpawn& exprSpawn) { auto [__0, __1] = exprSpawn; return mark_all_watched(watched, escaping); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return visit_expr(value, watched, escaping); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, body, __3] = exprWith; return visit_stmt_list(body, watched, visit_expr(resource, watched, escaping)); }
}, (*expression));
}
mlc::Array<mlc::String> non_escaping_params(std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> function_typed_names) noexcept{
  if ((function_typed_names.length() == 0))   {
    return empty_string_list();
  }
  auto escaping = visit_expr(body, function_typed_names, empty_string_list());
  return string_list_difference(function_typed_names, escaping);
}
mlc::Array<mlc::String> non_escaping_params_for_decl(std::shared_ptr<ast::Decl> declaration) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, __1, __2, parameters, __4, body, __6] = declFn; return [&]() {
auto watched = function_typed_param_names(parameters);
return non_escaping_params(body, watched);
}();
}
return empty_string_list();
std::abort();
}();
}
bool declaration_is_function_named(std::shared_ptr<ast::Decl> declaration, mlc::String function_name) noexcept{
  return [&]() -> bool {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, __1, __2, __3, __4, __5, __6] = declFn; return (name == function_name);
}
return false;
std::abort();
}();
}
mlc::Array<mlc::String> non_escaping_params_for_named_fn(ast::Program program, mlc::String function_name) noexcept{
  auto index = 0;
  while ((index < program.decls.length()))   {
    if (declaration_is_function_named(program.decls[index], function_name))     {
      return non_escaping_params_for_decl(program.decls[index]);
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return empty_string_list();
}
mlc::Array<mlc::String> collect_idents_used_as_values_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> names) noexcept{
  return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return string_list_add(names, name); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return names; },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return names; },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return names; },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return names; },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return names; },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return names; },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return names; },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return names; },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return names; },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return names; },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return collect_idents_used_as_values_in_expr(right, collect_idents_used_as_values_in_expr(left, names)); },
[&](const ast::ExprUn& exprUn) { auto [__0, operand, __2] = exprUn; return collect_idents_used_as_values_in_expr(operand, names); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return [&]() {
auto result = [&]() -> mlc::Array<mlc::String> {
auto __match_subject = callee;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [__0, __1] = exprIdent; return names;
}
return collect_idents_used_as_values_in_expr(callee, names);
std::abort();
}();
auto index = 0;
while ((index < arguments.length())) {
  (result = collect_idents_used_as_values_in_expr(arguments[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return [&]() {
auto result = collect_idents_used_as_values_in_expr(receiver, names);
auto index = 0;
while ((index < arguments.length())) {
  (result = collect_idents_used_as_values_in_expr(arguments[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return collect_idents_used_as_values_in_expr(object, names); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return collect_idents_used_as_values_in_expr(index_expression, collect_idents_used_as_values_in_expr(object, names)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return collect_idents_used_as_values_in_expr(else_branch, collect_idents_used_as_values_in_expr(then_branch, collect_idents_used_as_values_in_expr(condition, names))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return collect_idents_used_as_values_in_expr(result_expression, collect_idents_used_as_values_in_stmts(statements, names)); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return collect_idents_used_as_values_in_stmts(body, collect_idents_used_as_values_in_expr(condition, names)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterable, body, __3] = exprFor; return collect_idents_used_as_values_in_stmts(body, collect_idents_used_as_values_in_expr(iterable, names)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return [&]() {
auto result = collect_idents_used_as_values_in_expr(scrutinee, names);
auto index = 0;
while ((index < arms.length())) {
  auto arm = arms[index];
  if (arm->has_guard)   {
    (result = collect_idents_used_as_values_in_expr(arm->when_condition, result));
  }
  (result = collect_idents_used_as_values_in_expr(arm->body, result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return [&]() {
auto result = names;
auto index = 0;
while ((index < parts.length())) {
  (result = std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return [&]() {
auto field_result = result;
auto field_index = 0;
while ((field_index < field_values.length())) {
  (field_result = collect_idents_used_as_values_in_expr(field_values[field_index]->value, field_result));
  (field_index = mlc::arith::checked_add(field_index, 1));
}
return field_result;
}(); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return collect_idents_used_as_values_in_expr(spread_expression, result); }
}, parts[index]));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return [&]() {
auto result = collect_idents_used_as_values_in_expr(base, names);
auto index = 0;
while ((index < field_values.length())) {
  (result = collect_idents_used_as_values_in_expr(field_values[index]->value, result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return [&]() {
auto result = names;
auto index = 0;
while ((index < elements.length())) {
  (result = collect_idents_used_as_values_in_expr(elements[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return [&]() {
auto result = names;
auto index = 0;
while ((index < elements.length())) {
  (result = collect_idents_used_as_values_in_expr(elements[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}(); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return collect_idents_used_as_values_in_expr(inner, names); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return collect_idents_used_as_values_in_expr(body, names); },
[&](const ast::ExprSpawn& exprSpawn) { auto [body, __1] = exprSpawn; return collect_idents_used_as_values_in_stmts(body, names); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return collect_idents_used_as_values_in_expr(value, names); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, body, __3] = exprWith; return collect_idents_used_as_values_in_stmts(body, collect_idents_used_as_values_in_expr(resource, names)); }
}, (*expression));
}
mlc::Array<mlc::String> collect_idents_used_as_values_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> names) noexcept{
  auto result = names;
  auto index = 0;
  while ((index < statements.length()))   {
    (result = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return collect_idents_used_as_values_in_expr(value, result); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return collect_idents_used_as_values_in_expr(else_expression, collect_idents_used_as_values_in_expr(value, result)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return collect_idents_used_as_values_in_expr(value, result); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return collect_idents_used_as_values_in_expr(expression, result); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return result; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return result; },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return collect_idents_used_as_values_in_expr(expression, result); }
}, (*statements[index])));
    (index = mlc::arith::checked_add(index, 1));
  }
  return result;
}
mlc::Array<mlc::String> collect_idents_used_as_values_in_decl(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> names) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [__0, __1, __2, __3, __4, body, __6] = declFn; return collect_idents_used_as_values_in_expr(body, names);
}
if (std::holds_alternative<ast::DeclExtend>((*__match_subject))) {
const ast::DeclExtend& declExtend = std::get<ast::DeclExtend>((*__match_subject));
auto [__0, __1, methods, __3] = declExtend; return [&]() {
auto result = names;
auto index = 0;
while ((index < methods.length())) {
  (result = collect_idents_used_as_values_in_decl(methods[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}();
}
if (std::holds_alternative<ast::DeclTrait>((*__match_subject))) {
const ast::DeclTrait& declTrait = std::get<ast::DeclTrait>((*__match_subject));
auto [__0, __1, methods, __3] = declTrait; return [&]() {
auto result = names;
auto index = 0;
while ((index < methods.length())) {
  (result = collect_idents_used_as_values_in_decl(methods[index], result));
  (index = mlc::arith::checked_add(index, 1));
}
return result;
}();
}
if (std::holds_alternative<ast::DeclExported>((*__match_subject))) {
const ast::DeclExported& declExported = std::get<ast::DeclExported>((*__match_subject));
auto [inner] = declExported; return collect_idents_used_as_values_in_decl(inner, names);
}
return names;
std::abort();
}();
}
mlc::Array<mlc::String> function_names_used_as_values(ast::Program program) noexcept{
  auto names = empty_string_list();
  auto index = 0;
  while ((index < program.decls.length()))   {
    (names = collect_idents_used_as_values_in_decl(program.decls[index], names));
    (index = mlc::arith::checked_add(index, 1));
  }
  return names;
}
mlc::Array<mlc::String> non_escaping_params_for_decl_if_template_safe(std::shared_ptr<ast::Decl> declaration, mlc::Array<mlc::String> functions_used_as_values) noexcept{
  return [&]() -> mlc::Array<mlc::String> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclFn>((*__match_subject))) {
const ast::DeclFn& declFn = std::get<ast::DeclFn>((*__match_subject));
auto [name, __1, __2, __3, __4, __5, __6] = declFn; return (string_list_contains(functions_used_as_values, name) ? (empty_string_list()) : (non_escaping_params_for_decl(declaration)));
}
return empty_string_list();
std::abort();
}();
}

} // namespace escape_analysis
