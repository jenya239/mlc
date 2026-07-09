#define main mlc_user_main
#include "spawn_capture.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"

namespace spawn_capture {

using namespace ast;
using namespace names;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::Array<mlc::String> empty_names() noexcept{
  auto names = mlc::Array<mlc::String>{};
  return names;
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
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept{
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  return diagnostics;
}
ast::Diagnostic mutable_capture_diagnostic(mlc::String binding_name, ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(((mlc::String("mutable value '", 15) + binding_name) + mlc::String("' crosses concurrency boundary (spawn); use move or Sync-safe sharing", 69)), source_span, diagnostic_codes::diagnostic_code_e087());
}
mlc::Array<mlc::String> mutable_scope_from_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept{
  return parameters.fold(empty_names(), [=](mlc::Array<mlc::String> scope, std::shared_ptr<ast::Param> parameter) mutable { return (parameter->is_mut ? (names_add(scope, parameter->name)) : (scope)); });
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, source_span] = exprIdent; return ((names_contains(outer_mutable, name) && (!names_contains(local_bound, name))) ? (mlc::Array<ast::Diagnostic>{mutable_capture_diagnostic(name, source_span)}) : (empty_diagnostics())); },
[&](const ast::ExprUn& exprUn) { auto [operation, operand, __2] = exprUn; return [&]() -> mlc::Array<ast::Diagnostic> {
  if ((operation == mlc::String("move", 4)))   {
    return [&]() -> mlc::Array<ast::Diagnostic> {
auto __match_subject = operand;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [__0, __1] = exprIdent; return empty_diagnostics();
}
return free_mutable_capture_in_expr(operand, local_bound, outer_mutable);
std::abort();
}();
  } else   {
    return free_mutable_capture_in_expr(operand, local_bound, outer_mutable);
  }
}(); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return empty_diagnostics(); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return empty_diagnostics(); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return empty_diagnostics(); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return empty_diagnostics(); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return empty_diagnostics(); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return empty_diagnostics(); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return empty_diagnostics(); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return empty_diagnostics(); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return empty_diagnostics(); },
[&](const ast::ExprExtern& exprExtern) { auto [__0] = exprExtern; return empty_diagnostics(); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return ast::diagnostics_append(free_mutable_capture_in_expr(left, local_bound, outer_mutable), free_mutable_capture_in_expr(right, local_bound, outer_mutable)); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return ast::diagnostics_append(free_mutable_capture_in_expr(callee, local_bound, outer_mutable), free_mutable_capture_in_expr_list(arguments, local_bound, outer_mutable)); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return ast::diagnostics_append(free_mutable_capture_in_expr(receiver, local_bound, outer_mutable), free_mutable_capture_in_expr_list(arguments, local_bound, outer_mutable)); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return free_mutable_capture_in_expr(object, local_bound, outer_mutable); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(free_mutable_capture_in_expr(object, local_bound, outer_mutable), free_mutable_capture_in_expr(index_expression, local_bound, outer_mutable)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return ast::diagnostics_append(free_mutable_capture_in_expr(condition, local_bound, outer_mutable), ast::diagnostics_append(free_mutable_capture_in_expr(then_branch, local_bound, outer_mutable), free_mutable_capture_in_expr(else_branch, local_bound, outer_mutable))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return ast::diagnostics_append(free_mutable_capture_in_stmts(statements, local_bound, outer_mutable), free_mutable_capture_in_expr(result_expression, bindings_after_statements(statements, local_bound), outer_mutable)); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return ast::diagnostics_append(free_mutable_capture_in_expr(condition, local_bound, outer_mutable), free_mutable_capture_in_stmts(body, local_bound, outer_mutable)); },
[&](const ast::ExprFor& exprFor) { auto [variable_name, iterable, body, __3] = exprFor; return ast::diagnostics_append(free_mutable_capture_in_expr(iterable, local_bound, outer_mutable), free_mutable_capture_in_stmts(body, names_add(local_bound, variable_name), outer_mutable)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return ast::diagnostics_append(free_mutable_capture_in_expr(scrutinee, local_bound, outer_mutable), free_mutable_capture_in_match_arms(arms, local_bound, outer_mutable)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return free_mutable_capture_in_record_parts(parts, local_bound, outer_mutable); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(free_mutable_capture_in_expr(base, local_bound, outer_mutable), free_mutable_capture_in_field_values(field_values, local_bound, outer_mutable)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return free_mutable_capture_in_expr_list(elements, local_bound, outer_mutable); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return free_mutable_capture_in_expr_list(elements, local_bound, outer_mutable); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return free_mutable_capture_in_expr(inner, local_bound, outer_mutable); },
[&](const ast::ExprLambda& exprLambda) { auto [parameter_names, body, __2] = exprLambda; return free_mutable_capture_in_expr(body, names_add_all(local_bound, parameter_names), outer_mutable); },
[&](const ast::ExprSpawn& exprSpawn) { auto [__0, __1] = exprSpawn; return empty_diagnostics(); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return free_mutable_capture_in_expr(value, local_bound, outer_mutable); },
[&](const ast::ExprWith& exprWith) { auto [resource, binder, body, __3] = exprWith; return ast::diagnostics_append(free_mutable_capture_in_expr(resource, local_bound, outer_mutable), free_mutable_capture_in_stmts(body, names_add(local_bound, binder), outer_mutable)); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, free_mutable_capture_in_expr(expression, local_bound, outer_mutable)); });
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return field_values.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::FieldVal> field_value) mutable { return ast::diagnostics_append(diagnostics, free_mutable_capture_in_expr(field_value->value, local_bound, outer_mutable)); });
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return parts.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return ast::diagnostics_append(diagnostics, free_mutable_capture_in_field_values(field_values, local_bound, outer_mutable)); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return ast::diagnostics_append(diagnostics, free_mutable_capture_in_expr(spread_expression, local_bound, outer_mutable)); }
}, part); });
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto arm_bound = names_add_all(local_bound, names::pattern_bindings(arm->pattern));
auto guard_diagnostics = (arm->has_guard ? (free_mutable_capture_in_expr(arm->when_condition, arm_bound, outer_mutable)) : (empty_diagnostics()));
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(guard_diagnostics, free_mutable_capture_in_expr(arm->body, arm_bound, outer_mutable)));
}(); });
}
mlc::Array<mlc::String> statement_binding_names(std::shared_ptr<ast::Stmt> statement) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [name, __1, __2, __3, __4] = stmtLet; return mlc::Array<mlc::String>{name}; },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [pattern, __1, __2, __3, __4, __5, __6] = stmtLetPattern; return names::pattern_bindings(pattern); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [name, __1, __2, __3] = stmtLetConst; return mlc::Array<mlc::String>{name}; },
[&](const ast::StmtExpr& stmtExpr) { auto [__0, __1] = stmtExpr; return empty_names(); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return empty_names(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return empty_names(); },
[&](const ast::StmtReturn& stmtReturn) { auto [__0, __1] = stmtReturn; return empty_names(); }
}, (*statement));
}
bool statement_introduces_mutable(std::shared_ptr<ast::Stmt> statement) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, is_mut, __2, __3, __4] = stmtLet; return is_mut; },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, is_mut, __2, __3, __4, __5, __6] = stmtLetPattern; return is_mut; },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, __2, __3] = stmtLetConst; return false; },
[&](const ast::StmtExpr& stmtExpr) { auto [__0, __1] = stmtExpr; return false; },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return false; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return false; },
[&](const ast::StmtReturn& stmtReturn) { auto [__0, __1] = stmtReturn; return false; }
}, (*statement));
}
mlc::Array<mlc::String> bindings_after_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> local_bound) noexcept{
  return statements.fold(local_bound, [=](mlc::Array<mlc::String> bound, std::shared_ptr<ast::Stmt> statement) mutable { return names_add_all(bound, statement_binding_names(statement)); });
}
mlc::Array<ast::Diagnostic> free_mutable_capture_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> local_bound, mlc::Array<mlc::String> outer_mutable) noexcept{
  return statements.fold(SpawnCaptureStmtWalkState{empty_diagnostics(), local_bound}, [=](SpawnCaptureStmtWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return [&]() {
auto value_diagnostics = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return free_mutable_capture_in_expr(value, state.bound, outer_mutable); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return ast::diagnostics_append(free_mutable_capture_in_expr(value, state.bound, outer_mutable), free_mutable_capture_in_expr(else_expression, state.bound, outer_mutable)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return free_mutable_capture_in_expr(value, state.bound, outer_mutable); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return free_mutable_capture_in_expr(expression, state.bound, outer_mutable); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return empty_diagnostics(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return empty_diagnostics(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return free_mutable_capture_in_expr(expression, state.bound, outer_mutable); }
}, (*statement));
return SpawnCaptureStmtWalkState{ast::diagnostics_append(state.diagnostics, value_diagnostics), names_add_all(state.bound, statement_binding_names(statement))};
}(); }).diagnostics;
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_scope) noexcept{
  return std::visit(overloaded{[&](const ast::ExprSpawn& exprSpawn) { auto [body, __1] = exprSpawn; return ast::diagnostics_append(free_mutable_capture_in_stmts(body, empty_names(), mutable_scope), walk_spawn_sites_in_stmts(body, mutable_scope)); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return empty_diagnostics(); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return empty_diagnostics(); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return empty_diagnostics(); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return empty_diagnostics(); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return empty_diagnostics(); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return empty_diagnostics(); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return empty_diagnostics(); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return empty_diagnostics(); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return empty_diagnostics(); },
[&](const ast::ExprExtern& exprExtern) { auto [__0] = exprExtern; return empty_diagnostics(); },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return empty_diagnostics(); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return ast::diagnostics_append(walk_spawn_sites_in_expr(left, mutable_scope), walk_spawn_sites_in_expr(right, mutable_scope)); },
[&](const ast::ExprUn& exprUn) { auto [__0, operand, __2] = exprUn; return walk_spawn_sites_in_expr(operand, mutable_scope); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return ast::diagnostics_append(walk_spawn_sites_in_expr(callee, mutable_scope), walk_spawn_sites_in_expr_list(arguments, mutable_scope)); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return ast::diagnostics_append(walk_spawn_sites_in_expr(receiver, mutable_scope), walk_spawn_sites_in_expr_list(arguments, mutable_scope)); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return walk_spawn_sites_in_expr(object, mutable_scope); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(walk_spawn_sites_in_expr(object, mutable_scope), walk_spawn_sites_in_expr(index_expression, mutable_scope)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return ast::diagnostics_append(walk_spawn_sites_in_expr(condition, mutable_scope), ast::diagnostics_append(walk_spawn_sites_in_expr(then_branch, mutable_scope), walk_spawn_sites_in_expr(else_branch, mutable_scope))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return ast::diagnostics_append(walk_spawn_sites_in_stmts(statements, mutable_scope), walk_spawn_sites_in_expr(result_expression, mutable_scope_after_statements(statements, mutable_scope))); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return ast::diagnostics_append(walk_spawn_sites_in_expr(condition, mutable_scope), walk_spawn_sites_in_stmts(body, mutable_scope)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterable, body, __3] = exprFor; return ast::diagnostics_append(walk_spawn_sites_in_expr(iterable, mutable_scope), walk_spawn_sites_in_stmts(body, mutable_scope)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return ast::diagnostics_append(walk_spawn_sites_in_expr(scrutinee, mutable_scope), walk_spawn_sites_in_match_arms(arms, mutable_scope)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return walk_spawn_sites_in_record_parts(parts, mutable_scope); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(walk_spawn_sites_in_expr(base, mutable_scope), walk_spawn_sites_in_field_values(field_values, mutable_scope)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return walk_spawn_sites_in_expr_list(elements, mutable_scope); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return walk_spawn_sites_in_expr_list(elements, mutable_scope); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return walk_spawn_sites_in_expr(inner, mutable_scope); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return walk_spawn_sites_in_expr(body, empty_names()); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return walk_spawn_sites_in_expr(value, mutable_scope); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, body, __3] = exprWith; return ast::diagnostics_append(walk_spawn_sites_in_expr(resource, mutable_scope), walk_spawn_sites_in_stmts(body, mutable_scope)); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_scope) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(expression, mutable_scope)); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_scope) noexcept{
  return field_values.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::FieldVal> field_value) mutable { return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(field_value->value, mutable_scope)); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> mutable_scope) noexcept{
  return parts.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_field_values(field_values, mutable_scope)); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(spread_expression, mutable_scope)); }
}, part); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_scope) noexcept{
  return arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto guard_diagnostics = (arm->has_guard ? (walk_spawn_sites_in_expr(arm->when_condition, mutable_scope)) : (empty_diagnostics()));
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(guard_diagnostics, walk_spawn_sites_in_expr(arm->body, mutable_scope)));
}(); });
}
mlc::Array<mlc::String> mutable_scope_after_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope) noexcept{
  return statements.fold(mutable_scope, [=](mlc::Array<mlc::String> scope, std::shared_ptr<ast::Stmt> statement) mutable { return (statement_introduces_mutable(statement) ? (names_add_all(scope, statement_binding_names(statement))) : (scope)); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope) noexcept{
  return statements.fold(SpawnCaptureMutableWalkState{empty_diagnostics(), mutable_scope}, [=](SpawnCaptureMutableWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return [&]() {
auto value_diagnostics = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return walk_spawn_sites_in_expr(value, state.scope); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return ast::diagnostics_append(walk_spawn_sites_in_expr(value, state.scope), walk_spawn_sites_in_expr(else_expression, state.scope)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return walk_spawn_sites_in_expr(value, state.scope); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return walk_spawn_sites_in_expr(expression, state.scope); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return empty_diagnostics(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return empty_diagnostics(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return walk_spawn_sites_in_expr(expression, state.scope); }
}, (*statement));
auto scope_after = (statement_introduces_mutable(statement) ? (names_add_all(state.scope, statement_binding_names(statement))) : (state.scope));
return SpawnCaptureMutableWalkState{ast::diagnostics_append(state.diagnostics, value_diagnostics), scope_after};
}(); }).diagnostics;
}
mlc::Array<ast::Diagnostic> spawn_mutable_capture_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept{
  return walk_spawn_sites_in_expr(body, mutable_scope_from_parameters(parameters));
}

} // namespace spawn_capture
