#define main mlc_user_main
#include "region_escape.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"

namespace region_escape {

using namespace ast;
using namespace names;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_names() noexcept{
  auto names = mlc::Array<mlc::String>{};
  return names;
}
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept{
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  return diagnostics;
}
bool names_contains(mlc::Array<mlc::String> names, mlc::String sought) noexcept{
  return names.any([=](mlc::String entry) mutable { return (entry == sought); });
}
mlc::Array<mlc::String> names_add(mlc::Array<mlc::String> names, mlc::String name) noexcept{
  if ((name.length() == 0))   {
    return names;
  } else if (names_contains(names, name))   {
    return names;
  } else   {
    return names.concat(mlc::Array<mlc::String>{name});
  }
}
mlc::Array<mlc::String> names_add_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> additions) noexcept{
  return additions.fold(names, [=](mlc::Array<mlc::String> accumulated, mlc::String addition) mutable { return names_add(accumulated, addition); });
}
ast::Diagnostic region_escape_return_diagnostic(ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(mlc::String("region value escapes via return", 31), source_span, diagnostic_codes::diagnostic_code_e091());
}
ast::Diagnostic region_escape_closure_diagnostic(ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(mlc::String("region value escapes via closure capture", 40), source_span, diagnostic_codes::diagnostic_code_e091());
}
ast::Diagnostic region_escape_assign_diagnostic(ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(mlc::String("region value escapes via assignment outside region", 50), source_span, diagnostic_codes::diagnostic_code_e091());
}
ast::Diagnostic region_escape_field_diagnostic(mlc::String type_name, mlc::String field_name, ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code((((mlc::String("Region type not allowed as field '", 34) + field_name) + mlc::String("' of ", 5)) + type_name), source_span, diagnostic_codes::diagnostic_code_e091());
}
bool type_expression_contains_region(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return std::visit(overloaded{[&](const ast::TyGeneric& tyGeneric) { auto [name, type_arguments] = tyGeneric; return (((name == mlc::String("Region", 6)) || (name == mlc::String("RegionHandle", 12))) || type_arguments.any([=](std::shared_ptr<ast::TypeExpr> argument) mutable { return type_expression_contains_region(argument); })); },
[&](const ast::TyArray& tyArray) { auto [element_type] = tyArray; return type_expression_contains_region(element_type); },
[&](const ast::TyFn& tyFn) { auto [parameter_types, return_type] = tyFn; return (type_expression_contains_region(return_type) || parameter_types.any([=](std::shared_ptr<ast::TypeExpr> parameter) mutable { return type_expression_contains_region(parameter); })); },
[&](const ast::TyShared& tyShared) { auto [inner_type] = tyShared; return type_expression_contains_region(inner_type); },
[&](const ast::TyNamed& tyNamed) { auto [__0] = tyNamed; return false; },
[&](const ast::TyAssoc& tyAssoc) { auto [__0, __1] = tyAssoc; return false; },
[&](const ast::TyI32& tyI32) { return false; },
[&](const ast::TyString& tyString) { return false; },
[&](const ast::TyBool& tyBool) { return false; },
[&](const ast::TyUnit& tyUnit) { return false; }
}, (*type_expression));
}
bool expression_is_region_alloc(std::shared_ptr<ast::Expr> expression, mlc::String region_binder) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, method_name, __2, __3] = exprMethod; return ((method_name == mlc::String("alloc", 5)) && [&]() -> bool {
auto __match_subject = receiver;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (name == region_binder);
}
return false;
std::abort();
}());
}
return false;
std::abort();
}();
}
bool expression_mentions_region_value(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> region_values, mlc::String region_binder) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return names_contains(region_values, name);
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, method_name, method_arguments, __3] = exprMethod; return ((expression_is_region_alloc(expression, region_binder) || expression_mentions_region_value(receiver, region_values, region_binder)) || expression_list_mentions_region_value(method_arguments, region_values, region_binder));
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, call_arguments, __2] = exprCall; return (expression_mentions_region_value(callee, region_values, region_binder) || expression_list_mentions_region_value(call_arguments, region_values, region_binder));
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [__0, left, right, __3] = exprBin; return (expression_mentions_region_value(left, region_values, region_binder) || expression_mentions_region_value(right, region_values, region_binder));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, inner, __2] = exprUn; return expression_mentions_region_value(inner, region_values, region_binder);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, __1, __2] = exprField; return expression_mentions_region_value(object, region_values, region_binder);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, __2] = exprIndex; return (expression_mentions_region_value(object, region_values, region_binder) || expression_mentions_region_value(index_expression, region_values, region_binder));
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3] = exprIf; return ((expression_mentions_region_value(condition, region_values, region_binder) || expression_mentions_region_value(then_expression, region_values, region_binder)) || expression_mentions_region_value(else_expression, region_values, region_binder));
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return expression_list_mentions_region_value(elements, region_values, region_binder);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return expression_list_mentions_region_value(elements, region_values, region_binder);
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, value, __2] = exprNamedArg; return expression_mentions_region_value(value, region_values, region_binder);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner, __1] = exprQuestion; return expression_mentions_region_value(inner, region_values, region_binder);
}
return false;
std::abort();
}();
}
bool expression_list_mentions_region_value(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> region_values, mlc::String region_binder) noexcept{
  return expressions.any([=](std::shared_ptr<ast::Expr> expression) mutable { return expression_mentions_region_value(expression, region_values, region_binder); });
}
bool lambda_captures_region_value(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, mlc::Array<mlc::String> region_values) noexcept{
  return region_values.any([=](mlc::String region_value_name) mutable { return ((!names_contains(parameter_names, region_value_name)) && expression_uses_ident(body, region_value_name)); });
}
bool expression_uses_ident(std::shared_ptr<ast::Expr> expression, mlc::String sought) noexcept{
  return [&]() -> bool {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (name == sought);
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, __1, method_arguments, __3] = exprMethod; return (expression_uses_ident(receiver, sought) || expression_list_uses_ident(method_arguments, sought));
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, call_arguments, __2] = exprCall; return (expression_uses_ident(callee, sought) || expression_list_uses_ident(call_arguments, sought));
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [__0, left, right, __3] = exprBin; return (expression_uses_ident(left, sought) || expression_uses_ident(right, sought));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, inner, __2] = exprUn; return expression_uses_ident(inner, sought);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, __1, __2] = exprField; return expression_uses_ident(object, sought);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, __2] = exprIndex; return (expression_uses_ident(object, sought) || expression_uses_ident(index_expression, sought));
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3] = exprIf; return ((expression_uses_ident(condition, sought) || expression_uses_ident(then_expression, sought)) || expression_uses_ident(else_expression, sought));
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return ((!names_contains(parameter_names, sought)) && expression_uses_ident(body, sought));
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return expression_list_uses_ident(elements, sought);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return expression_list_uses_ident(elements, sought);
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return (statements_use_ident(statements, sought) || expression_uses_ident(result_expression, sought));
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, value, __2] = exprNamedArg; return expression_uses_ident(value, sought);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner, __1] = exprQuestion; return expression_uses_ident(inner, sought);
}
return false;
std::abort();
}();
}
bool expression_list_uses_ident(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String sought) noexcept{
  return expressions.any([=](std::shared_ptr<ast::Expr> expression) mutable { return expression_uses_ident(expression, sought); });
}
bool statements_use_ident(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String sought) noexcept{
  return statements.any([=](std::shared_ptr<ast::Stmt> statement) mutable { return [&]() -> bool {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [__0, __1, __2, value, __4] = stmtLet; return expression_uses_ident(value, sought);
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [__0, __1, value, __3] = stmtLetConst; return expression_uses_ident(value, sought);
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [__0, __1, __2, value, __4, else_body, __6] = stmtLetPattern; return (expression_uses_ident(value, sought) || expression_uses_ident(else_body, sought));
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return expression_uses_ident(expression, sought);
}
if (std::holds_alternative<ast::StmtReturn>((*__match_subject))) {
const ast::StmtReturn& stmtReturn = std::get<ast::StmtReturn>((*__match_subject));
auto [expression, __1] = stmtReturn; return expression_uses_ident(expression, sought);
}
return false;
std::abort();
}(); });
}
mlc::Array<ast::Diagnostic> walk_region_expression(std::shared_ptr<ast::Expr> expression, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, source_span] = exprLambda; return [&]() {
auto diagnostics = empty_diagnostics();
if (lambda_captures_region_value(parameter_names, body, region_values)) {
  (diagnostics = diagnostics.concat(mlc::Array<ast::Diagnostic>{region_escape_closure_diagnostic(source_span)}));
}
return ast::diagnostics_append(diagnostics, walk_region_expression(body, region_binder, region_values, region_locals, outer_bound));
}();
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [operation, left, right, source_span] = exprBin; return [&]() {
auto diagnostics = empty_diagnostics();
if ((operation == mlc::String("=", 1))) {
  [&]() {
auto __match_subject = left;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [target_name, __1] = exprIdent; [&]() -> void {
  if (((!names_contains(region_locals, target_name)) && expression_mentions_region_value(right, region_values, region_binder)))   {
(diagnostics = diagnostics.concat(mlc::Array<ast::Diagnostic>{region_escape_assign_diagnostic(source_span)}));

  }
}();
return;
}
std::make_tuple();
return;
std::abort();
}();
}
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(walk_region_expression(left, region_binder, region_values, region_locals, outer_bound), walk_region_expression(right, region_binder, region_values, region_locals, outer_bound)));
}();
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, __1, method_arguments, __3] = exprMethod; return ast::diagnostics_append(walk_region_expression(receiver, region_binder, region_values, region_locals, outer_bound), walk_region_expression_list(method_arguments, region_binder, region_values, region_locals, outer_bound));
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, call_arguments, __2] = exprCall; return ast::diagnostics_append(walk_region_expression(callee, region_binder, region_values, region_locals, outer_bound), walk_region_expression_list(call_arguments, region_binder, region_values, region_locals, outer_bound));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, inner, __2] = exprUn; return walk_region_expression(inner, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, __1, __2] = exprField; return walk_region_expression(object, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(walk_region_expression(object, region_binder, region_values, region_locals, outer_bound), walk_region_expression(index_expression, region_binder, region_values, region_locals, outer_bound));
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3] = exprIf; return ast::diagnostics_append(walk_region_expression(condition, region_binder, region_values, region_locals, outer_bound), ast::diagnostics_append(walk_region_expression(then_expression, region_binder, region_values, region_locals, outer_bound), walk_region_expression(else_expression, region_binder, region_values, region_locals, outer_bound)));
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return ast::diagnostics_append(walk_region_statements(statements, region_binder, region_values, region_locals, outer_bound).diagnostics, walk_region_expression(result_expression, region_binder, region_values, region_locals, outer_bound));
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return walk_region_expression_list(elements, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return walk_region_expression_list(elements, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [subject, arms, __2] = exprMatch; return ast::diagnostics_append(walk_region_expression(subject, region_binder, region_values, region_locals, outer_bound), walk_region_match_arms(arms, region_binder, region_values, region_locals, outer_bound));
}
if (std::holds_alternative<ast::ExprRecord>((*__match_subject))) {
const ast::ExprRecord& exprRecord = std::get<ast::ExprRecord>((*__match_subject));
auto [__0, parts, __2] = exprRecord; return walk_region_record_parts(parts, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, value, __2] = exprNamedArg; return walk_region_expression(value, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner, __1] = exprQuestion; return walk_region_expression(inner, region_binder, region_values, region_locals, outer_bound);
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [resource, binder, statements, __3] = exprWith; return ast::diagnostics_append(walk_region_expression(resource, region_binder, region_values, region_locals, outer_bound), walk_region_statements(statements, region_binder, region_values, names_add(region_locals, binder), outer_bound).diagnostics);
}
if (std::holds_alternative<ast::ExprScope>((*__match_subject))) {
const ast::ExprScope& exprScope = std::get<ast::ExprScope>((*__match_subject));
auto [binder, statements, __2] = exprScope; return walk_region_statements(statements, region_binder, region_values, names_add(region_locals, binder), outer_bound).diagnostics;
}
if (std::holds_alternative<ast::ExprRegion>((*__match_subject))) {
const ast::ExprRegion& exprRegion = std::get<ast::ExprRegion>((*__match_subject));
auto [inner_binder, statements, __2] = exprRegion; return walk_region_block(inner_binder, statements, names_add_all(outer_bound, region_locals));
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [statements, __1] = exprSpawn; return walk_region_statements(statements, region_binder, region_values, region_locals, outer_bound).diagnostics;
}
return empty_diagnostics();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> walk_region_expression_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, walk_region_expression(expression, region_binder, region_values, region_locals, outer_bound)); });
}
mlc::Array<ast::Diagnostic> walk_region_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept{
  return parts.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return ast::diagnostics_append(diagnostics, field_values.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> field_diagnostics, std::shared_ptr<ast::FieldVal> field_value) mutable { return ast::diagnostics_append(field_diagnostics, walk_region_expression(field_value->value, region_binder, region_values, region_locals, outer_bound)); })); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return ast::diagnostics_append(diagnostics, walk_region_expression(spread_expression, region_binder, region_values, region_locals, outer_bound)); }
}, part); });
}
mlc::Array<ast::Diagnostic> walk_region_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept{
  return arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto arm_locals = names_add_all(region_locals, names::pattern_bindings(arm->pattern));
auto guard_diagnostics = (arm->has_guard ? (walk_region_expression(arm->when_condition, region_binder, region_values, arm_locals, outer_bound)) : (empty_diagnostics()));
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(guard_diagnostics, walk_region_expression(arm->body, region_binder, region_values, arm_locals, outer_bound)));
}(); });
}
RegionEscapeWalkState walk_region_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String region_binder, mlc::Array<mlc::String> region_values, mlc::Array<mlc::String> region_locals, mlc::Array<mlc::String> outer_bound) noexcept{
  return statements.fold(RegionEscapeWalkState{empty_diagnostics(), region_values, region_locals}, [=](RegionEscapeWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return walk_region_statement(statement, region_binder, state, outer_bound); });
}
RegionEscapeWalkState walk_region_statement(std::shared_ptr<ast::Stmt> statement, mlc::String region_binder, RegionEscapeWalkState state, mlc::Array<mlc::String> outer_bound) noexcept{
  return [&]() -> RegionEscapeWalkState {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, __1, __2, value, __4] = stmtLet; return [&]() {
auto diagnostics = ast::diagnostics_append(state.diagnostics, walk_region_expression(value, region_binder, state.region_values, state.region_locals, outer_bound));
auto region_values = state.region_values;
if ((expression_is_region_alloc(value, region_binder) || expression_mentions_region_value(value, state.region_values, region_binder))) {
  (region_values = names_add(region_values, name));
}
return RegionEscapeWalkState{diagnostics, region_values, names_add(state.region_locals, name)};
}();
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [name, __1, value, __3] = stmtLetConst; return [&]() {
auto diagnostics = ast::diagnostics_append(state.diagnostics, walk_region_expression(value, region_binder, state.region_values, state.region_locals, outer_bound));
auto region_values = state.region_values;
if ((expression_is_region_alloc(value, region_binder) || expression_mentions_region_value(value, state.region_values, region_binder))) {
  (region_values = names_add(region_values, name));
}
return RegionEscapeWalkState{diagnostics, region_values, names_add(state.region_locals, name)};
}();
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [pattern, __1, __2, value, __4, else_body, __6] = stmtLetPattern; return [&]() {
auto binding_names = names::pattern_bindings(pattern);
auto diagnostics = ast::diagnostics_append(state.diagnostics, ast::diagnostics_append(walk_region_expression(value, region_binder, state.region_values, state.region_locals, outer_bound), walk_region_expression(else_body, region_binder, state.region_values, state.region_locals, outer_bound)));
auto region_values = state.region_values;
if ((expression_is_region_alloc(value, region_binder) || expression_mentions_region_value(value, state.region_values, region_binder))) {
  (region_values = names_add_all(region_values, binding_names));
}
return RegionEscapeWalkState{diagnostics, region_values, names_add_all(state.region_locals, binding_names)};
}();
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return RegionEscapeWalkState{ast::diagnostics_append(state.diagnostics, walk_region_expression(expression, region_binder, state.region_values, state.region_locals, outer_bound)), state.region_values, state.region_locals};
}
if (std::holds_alternative<ast::StmtReturn>((*__match_subject))) {
const ast::StmtReturn& stmtReturn = std::get<ast::StmtReturn>((*__match_subject));
auto [expression, source_span] = stmtReturn; return [&]() {
auto diagnostics = state.diagnostics;
if (expression_mentions_region_value(expression, state.region_values, region_binder)) {
  (diagnostics = diagnostics.concat(mlc::Array<ast::Diagnostic>{region_escape_return_diagnostic(source_span)}));
}
return RegionEscapeWalkState{ast::diagnostics_append(diagnostics, walk_region_expression(expression, region_binder, state.region_values, state.region_locals, outer_bound)), state.region_values, state.region_locals};
}();
}
return state;
std::abort();
}();
}
mlc::Array<ast::Diagnostic> walk_region_block(mlc::String region_binder, mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> outer_bound) noexcept{
  return walk_region_statements(statements, region_binder, empty_names(), names_add(empty_names(), region_binder), outer_bound).diagnostics;
}
mlc::Array<ast::Diagnostic> walk_expression_for_regions(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> outer_bound) noexcept{
  return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = expression;
if (std::holds_alternative<ast::ExprRegion>((*__match_subject))) {
const ast::ExprRegion& exprRegion = std::get<ast::ExprRegion>((*__match_subject));
auto [binder, statements, __2] = exprRegion; return walk_region_block(binder, statements, outer_bound);
}
if (std::holds_alternative<ast::ExprBlock>((*__match_subject))) {
const ast::ExprBlock& exprBlock = std::get<ast::ExprBlock>((*__match_subject));
auto [statements, result_expression, __2] = exprBlock; return ast::diagnostics_append(walk_statements_for_regions(statements, outer_bound), walk_expression_for_regions(result_expression, outer_bound));
}
if (std::holds_alternative<ast::ExprLambda>((*__match_subject))) {
const ast::ExprLambda& exprLambda = std::get<ast::ExprLambda>((*__match_subject));
auto [parameter_names, body, __2] = exprLambda; return walk_expression_for_regions(body, names_add_all(outer_bound, parameter_names));
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, __1, method_arguments, __3] = exprMethod; return ast::diagnostics_append(walk_expression_for_regions(receiver, outer_bound), walk_expression_list_for_regions(method_arguments, outer_bound));
}
if (std::holds_alternative<ast::ExprCall>((*__match_subject))) {
const ast::ExprCall& exprCall = std::get<ast::ExprCall>((*__match_subject));
auto [callee, call_arguments, __2] = exprCall; return ast::diagnostics_append(walk_expression_for_regions(callee, outer_bound), walk_expression_list_for_regions(call_arguments, outer_bound));
}
if (std::holds_alternative<ast::ExprBin>((*__match_subject))) {
const ast::ExprBin& exprBin = std::get<ast::ExprBin>((*__match_subject));
auto [__0, left, right, __3] = exprBin; return ast::diagnostics_append(walk_expression_for_regions(left, outer_bound), walk_expression_for_regions(right, outer_bound));
}
if (std::holds_alternative<ast::ExprUn>((*__match_subject))) {
const ast::ExprUn& exprUn = std::get<ast::ExprUn>((*__match_subject));
auto [__0, inner, __2] = exprUn; return walk_expression_for_regions(inner, outer_bound);
}
if (std::holds_alternative<ast::ExprField>((*__match_subject))) {
const ast::ExprField& exprField = std::get<ast::ExprField>((*__match_subject));
auto [object, __1, __2] = exprField; return walk_expression_for_regions(object, outer_bound);
}
if (std::holds_alternative<ast::ExprIndex>((*__match_subject))) {
const ast::ExprIndex& exprIndex = std::get<ast::ExprIndex>((*__match_subject));
auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(walk_expression_for_regions(object, outer_bound), walk_expression_for_regions(index_expression, outer_bound));
}
if (std::holds_alternative<ast::ExprIf>((*__match_subject))) {
const ast::ExprIf& exprIf = std::get<ast::ExprIf>((*__match_subject));
auto [condition, then_expression, else_expression, __3] = exprIf; return ast::diagnostics_append(walk_expression_for_regions(condition, outer_bound), ast::diagnostics_append(walk_expression_for_regions(then_expression, outer_bound), walk_expression_for_regions(else_expression, outer_bound)));
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [elements, __1] = exprArray; return walk_expression_list_for_regions(elements, outer_bound);
}
if (std::holds_alternative<ast::ExprTuple>((*__match_subject))) {
const ast::ExprTuple& exprTuple = std::get<ast::ExprTuple>((*__match_subject));
auto [elements, __1] = exprTuple; return walk_expression_list_for_regions(elements, outer_bound);
}
if (std::holds_alternative<ast::ExprMatch>((*__match_subject))) {
const ast::ExprMatch& exprMatch = std::get<ast::ExprMatch>((*__match_subject));
auto [subject, arms, __2] = exprMatch; return ast::diagnostics_append(walk_expression_for_regions(subject, outer_bound), arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return ast::diagnostics_append(diagnostics, walk_expression_for_regions(arm->body, names_add_all(outer_bound, names::pattern_bindings(arm->pattern)))); }));
}
if (std::holds_alternative<ast::ExprWith>((*__match_subject))) {
const ast::ExprWith& exprWith = std::get<ast::ExprWith>((*__match_subject));
auto [resource, binder, statements, __3] = exprWith; return ast::diagnostics_append(walk_expression_for_regions(resource, outer_bound), walk_statements_for_regions(statements, names_add(outer_bound, binder)));
}
if (std::holds_alternative<ast::ExprScope>((*__match_subject))) {
const ast::ExprScope& exprScope = std::get<ast::ExprScope>((*__match_subject));
auto [binder, statements, __2] = exprScope; return walk_statements_for_regions(statements, names_add(outer_bound, binder));
}
if (std::holds_alternative<ast::ExprSpawn>((*__match_subject))) {
const ast::ExprSpawn& exprSpawn = std::get<ast::ExprSpawn>((*__match_subject));
auto [statements, __1] = exprSpawn; return walk_statements_for_regions(statements, outer_bound);
}
if (std::holds_alternative<ast::ExprNamedArg>((*__match_subject))) {
const ast::ExprNamedArg& exprNamedArg = std::get<ast::ExprNamedArg>((*__match_subject));
auto [__0, value, __2] = exprNamedArg; return walk_expression_for_regions(value, outer_bound);
}
if (std::holds_alternative<ast::ExprQuestion>((*__match_subject))) {
const ast::ExprQuestion& exprQuestion = std::get<ast::ExprQuestion>((*__match_subject));
auto [inner, __1] = exprQuestion; return walk_expression_for_regions(inner, outer_bound);
}
return empty_diagnostics();
std::abort();
}();
}
mlc::Array<ast::Diagnostic> walk_expression_list_for_regions(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> outer_bound) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, walk_expression_for_regions(expression, outer_bound)); });
}
mlc::Array<ast::Diagnostic> walk_statements_for_regions(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> outer_bound) noexcept{
  return statements.fold(RegionEscapeWalkState{empty_diagnostics(), empty_names(), outer_bound}, [=](RegionEscapeWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return [&]() {
auto value_diagnostics = [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, __1, __2, value, __4] = stmtLet; return walk_expression_for_regions(value, state.region_locals);
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [name, __1, value, __3] = stmtLetConst; return walk_expression_for_regions(value, state.region_locals);
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [__0, __1, __2, value, __4, else_body, __6] = stmtLetPattern; return ast::diagnostics_append(walk_expression_for_regions(value, state.region_locals), walk_expression_for_regions(else_body, state.region_locals));
}
if (std::holds_alternative<ast::StmtExpr>((*__match_subject))) {
const ast::StmtExpr& stmtExpr = std::get<ast::StmtExpr>((*__match_subject));
auto [expression, __1] = stmtExpr; return walk_expression_for_regions(expression, state.region_locals);
}
if (std::holds_alternative<ast::StmtReturn>((*__match_subject))) {
const ast::StmtReturn& stmtReturn = std::get<ast::StmtReturn>((*__match_subject));
auto [expression, __1] = stmtReturn; return walk_expression_for_regions(expression, state.region_locals);
}
return empty_diagnostics();
std::abort();
}();
auto next_locals = [&]() -> mlc::Array<mlc::String> {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, __1, __2, __3, __4] = stmtLet; return names_add(state.region_locals, name);
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [name, __1, __2, __3] = stmtLetConst; return names_add(state.region_locals, name);
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [pattern, __1, __2, __3, __4, __5, __6] = stmtLetPattern; return names_add_all(state.region_locals, names::pattern_bindings(pattern));
}
return state.region_locals;
std::abort();
}();
return RegionEscapeWalkState{ast::diagnostics_append(state.diagnostics, value_diagnostics), state.region_values, next_locals};
}(); }).diagnostics;
}
mlc::Array<ast::Diagnostic> field_def_region_diagnostics(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> fields) noexcept{
  return fields.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::FieldDef> field) mutable { return (type_expression_contains_region(field->type_value) ? (diagnostics.concat(mlc::Array<ast::Diagnostic>{region_escape_field_diagnostic(type_name, field->name, ast::span_unknown())})) : (diagnostics)); });
}
mlc::Array<ast::Diagnostic> variant_region_field_diagnostics(mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept{
  return std::visit(overloaded{[&](const ast::VarRecord& varRecord) { auto [__0, fields, __2] = varRecord; return field_def_region_diagnostics(type_name, fields); },
[&](const ast::VarTuple& varTuple) { auto [__0, payload_types, __2] = varTuple; return (payload_types.any([=](std::shared_ptr<ast::TypeExpr> payload) mutable { return type_expression_contains_region(payload); }) ? (mlc::Array<ast::Diagnostic>{region_escape_field_diagnostic(type_name, mlc::String("<tuple>", 7), ast::span_unknown())}) : (empty_diagnostics())); },
[&](const ast::VarUnit& varUnit) { auto [__0, __1] = varUnit; return empty_diagnostics(); }
}, (*variant));
}
mlc::Array<ast::Diagnostic> region_field_type_diagnostics(ast::Program program) noexcept{
  return program.decls.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Decl> declaration) mutable { return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = ast::decl_inner(declaration);
if (std::holds_alternative<ast::DeclType>((*__match_subject))) {
const ast::DeclType& declType = std::get<ast::DeclType>((*__match_subject));
auto [type_name, __1, variants, __3, __4] = declType; return ast::diagnostics_append(diagnostics, variants.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> variant_diagnostics, std::shared_ptr<ast::TypeVariant> variant) mutable { return ast::diagnostics_append(variant_diagnostics, variant_region_field_diagnostics(type_name, variant)); }));
}
return diagnostics;
std::abort();
}(); });
}
mlc::Array<ast::Diagnostic> region_escape_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept{
  auto outer_bound = parameters.fold(empty_names(), [=](mlc::Array<mlc::String> names, std::shared_ptr<ast::Param> parameter) mutable { return names_add(names, ast::param_name(parameter)); });
  return walk_expression_for_regions(body, outer_bound);
}

} // namespace region_escape
