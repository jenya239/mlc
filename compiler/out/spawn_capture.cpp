#define main mlc_user_main
#include "spawn_capture.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "registry.hpp"
#include "send_safe.hpp"

namespace spawn_capture {

using namespace ast;
using namespace names;
using namespace diagnostic_codes;
using namespace registry;
using namespace send_safe;
using namespace ast_tokens;

bool type_is_sync_safe_shared(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return [&]() -> bool {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TGeneric>((*__match_subject))) {
const registry::TGeneric& tGeneric = std::get<registry::TGeneric>((*__match_subject));
auto [type_name, type_arguments] = tGeneric; return (((type_name == mlc::String("Mutex", 5)) && (type_arguments.length() == 1)) ? (true) : ((((type_name == mlc::String("Arc", 3)) && (type_arguments.length() == 1)) ? (send_safe::type_is_sync(type_arguments[0], registry)) : (false))));
}
return false;
std::abort();
}();
}
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
ast::Diagnostic bare_spawn_diagnostic(ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(mlc::String("bare spawn discards Task and blocks in destructor; bind with let or use scope", 77), source_span, diagnostic_codes::diagnostic_code_e089());
}
bool type_annotation_is_elided(std::shared_ptr<ast::TypeExpr> type_expression) noexcept{
  return [&]() -> bool {
auto __match_subject = type_expression;
if (std::holds_alternative<ast::TyUnit>((*__match_subject))) {
const ast::TyUnit& tyUnit = std::get<ast::TyUnit>((*__match_subject));
return true;
}
return false;
std::abort();
}();
}
std::shared_ptr<registry::Type> type_hint_from_value(std::shared_ptr<ast::Expr> value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{
  return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = value;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return (type_environment.has(name) ? (type_environment.get(name)) : (std::make_shared<registry::Type>(registry::TUnknown{})));
}
if (std::holds_alternative<ast::ExprInt>((*__match_subject))) {
const ast::ExprInt& exprInt = std::get<ast::ExprInt>((*__match_subject));
auto [__0, __1] = exprInt; return std::make_shared<registry::Type>(registry::TI32{});
}
if (std::holds_alternative<ast::ExprArray>((*__match_subject))) {
const ast::ExprArray& exprArray = std::get<ast::ExprArray>((*__match_subject));
auto [__0, __1] = exprArray; return std::make_shared<registry::Type>(registry::TArray{std::make_shared<registry::Type>(registry::TUnknown{})});
}
if (std::holds_alternative<ast::ExprMethod>((*__match_subject))) {
const ast::ExprMethod& exprMethod = std::get<ast::ExprMethod>((*__match_subject));
auto [receiver, method_name, arguments, __3] = exprMethod; return [&]() -> std::shared_ptr<registry::Type> {
  if ((method_name == mlc::String("new", 3)))   {
    return [&]() -> std::shared_ptr<registry::Type> {
auto __match_subject = receiver;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [type_name, __1] = exprIdent; return ((type_name == mlc::String("Mutex", 5)) ? (std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Mutex", 5), mlc::Array<std::shared_ptr<registry::Type>>{((arguments.length() > 0) ? (type_hint_from_value(arguments[0], type_environment, registry)) : (std::make_shared<registry::Type>(registry::TUnknown{})))}})) : (((type_name == mlc::String("Arc", 3)) ? (std::make_shared<registry::Type>(registry::TGeneric{mlc::String("Arc", 3), mlc::Array<std::shared_ptr<registry::Type>>{((arguments.length() > 0) ? (type_hint_from_value(arguments[0], type_environment, registry)) : (std::make_shared<registry::Type>(registry::TUnknown{})))}})) : (std::make_shared<registry::Type>(registry::TUnknown{})))));
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}();
}
return std::make_shared<registry::Type>(registry::TUnknown{});
std::abort();
}();
}
std::shared_ptr<registry::Type> resolve_binding_type(std::shared_ptr<ast::TypeExpr> type_expression, std::shared_ptr<ast::Expr> value, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{
  if (type_annotation_is_elided(type_expression))   {
    return type_hint_from_value(value, type_environment, registry);
  } else   {
    return registry::type_from_annotation_with_registry(type_expression, registry);
  }
}
mlc::Array<mlc::String> mutable_scope_from_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept{
  return parameters.fold(empty_names(), [=](mlc::Array<mlc::String> binding_scope, std::shared_ptr<ast::Param> parameter) mutable { return [&]() {
if ((!parameter->is_mut)) {
  return binding_scope;
}
auto parameter_type = registry::type_from_annotation_with_registry(parameter->type_value, registry);
return (type_is_sync_safe_shared(parameter_type, registry) ? (binding_scope) : (names_add(binding_scope, parameter->name)));
}(); });
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_from_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters, registry::TypeRegistry registry) noexcept{
  auto type_environment = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
  auto index = 0;
  while ((index < parameters.length()))   {
    auto parameter = parameters[index];
    type_environment.set(parameter->name, registry::type_from_annotation_with_registry(parameter->type_value, registry));
    (index = mlc::arith::checked_add(index, 1));
  }
  return type_environment;
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
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return empty_diagnostics(); },
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
[&](const ast::ExprWith& exprWith) { auto [resource, binder, body, __3] = exprWith; return ast::diagnostics_append(free_mutable_capture_in_expr(resource, local_bound, outer_mutable), free_mutable_capture_in_stmts(body, names_add(local_bound, binder), outer_mutable)); },
[&](const ast::ExprScope& exprScope) { auto [binder, body, __2] = exprScope; return free_mutable_capture_in_stmts(body, names_add(local_bound, binder), outer_mutable); },
[&](const ast::ExprRegion& exprRegion) { auto [binder, body, __2] = exprRegion; return free_mutable_capture_in_stmts(body, names_add(local_bound, binder), outer_mutable); }
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
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return std::visit(overloaded{[&](const ast::ExprSpawn& exprSpawn) { auto [body, __1] = exprSpawn; return ast::diagnostics_append(free_mutable_capture_in_stmts(body, empty_names(), mutable_scope), walk_spawn_sites_in_stmts(body, mutable_scope, capture_context)); },
[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return empty_diagnostics(); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return empty_diagnostics(); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return empty_diagnostics(); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return empty_diagnostics(); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return empty_diagnostics(); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return empty_diagnostics(); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return empty_diagnostics(); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return empty_diagnostics(); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return empty_diagnostics(); },
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return empty_diagnostics(); },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return empty_diagnostics(); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return ast::diagnostics_append(walk_spawn_sites_in_expr(left, mutable_scope, capture_context), walk_spawn_sites_in_expr(right, mutable_scope, capture_context)); },
[&](const ast::ExprUn& exprUn) { auto [__0, operand, __2] = exprUn; return walk_spawn_sites_in_expr(operand, mutable_scope, capture_context); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return ast::diagnostics_append(walk_spawn_sites_in_expr(callee, mutable_scope, capture_context), walk_spawn_sites_in_expr_list(arguments, mutable_scope, capture_context)); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return ast::diagnostics_append(walk_spawn_sites_in_expr(receiver, mutable_scope, capture_context), walk_spawn_sites_in_expr_list(arguments, mutable_scope, capture_context)); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return walk_spawn_sites_in_expr(object, mutable_scope, capture_context); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(walk_spawn_sites_in_expr(object, mutable_scope, capture_context), walk_spawn_sites_in_expr(index_expression, mutable_scope, capture_context)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return ast::diagnostics_append(walk_spawn_sites_in_expr(condition, mutable_scope, capture_context), ast::diagnostics_append(walk_spawn_sites_in_expr(then_branch, mutable_scope, capture_context), walk_spawn_sites_in_expr(else_branch, mutable_scope, capture_context))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return [&]() {
auto statements_state = walk_spawn_sites_in_stmts_state(statements, mutable_scope, capture_context);
return ast::diagnostics_append(statements_state.diagnostics, walk_spawn_sites_in_expr(result_expression, statements_state.binding_scope, SpawnCaptureContext{capture_context.registry, statements_state.type_environment}));
}(); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return ast::diagnostics_append(walk_spawn_sites_in_expr(condition, mutable_scope, capture_context), walk_spawn_sites_in_stmts(body, mutable_scope, capture_context)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterable, body, __3] = exprFor; return ast::diagnostics_append(walk_spawn_sites_in_expr(iterable, mutable_scope, capture_context), walk_spawn_sites_in_stmts(body, mutable_scope, capture_context)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return ast::diagnostics_append(walk_spawn_sites_in_expr(scrutinee, mutable_scope, capture_context), walk_spawn_sites_in_match_arms(arms, mutable_scope, capture_context)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return walk_spawn_sites_in_record_parts(parts, mutable_scope, capture_context); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(walk_spawn_sites_in_expr(base, mutable_scope, capture_context), walk_spawn_sites_in_field_values(field_values, mutable_scope, capture_context)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return walk_spawn_sites_in_expr_list(elements, mutable_scope, capture_context); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return walk_spawn_sites_in_expr_list(elements, mutable_scope, capture_context); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return walk_spawn_sites_in_expr(inner, mutable_scope, capture_context); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return walk_spawn_sites_in_expr(body, empty_names(), capture_context); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return walk_spawn_sites_in_expr(value, mutable_scope, capture_context); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, body, __3] = exprWith; return ast::diagnostics_append(walk_spawn_sites_in_expr(resource, mutable_scope, capture_context), walk_spawn_sites_in_stmts(body, mutable_scope, capture_context)); },
[&](const ast::ExprScope& exprScope) { auto [__0, body, __2] = exprScope; return walk_spawn_sites_in_stmts(body, mutable_scope, capture_context); },
[&](const ast::ExprRegion& exprRegion) { auto [__0, body, __2] = exprRegion; return walk_spawn_sites_in_stmts(body, mutable_scope, capture_context); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(expression, mutable_scope, capture_context)); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return field_values.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::FieldVal> field_value) mutable { return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(field_value->value, mutable_scope, capture_context)); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return parts.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_field_values(field_values, mutable_scope, capture_context)); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return ast::diagnostics_append(diagnostics, walk_spawn_sites_in_expr(spread_expression, mutable_scope, capture_context)); }
}, part); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto guard_diagnostics = (arm->has_guard ? (walk_spawn_sites_in_expr(arm->when_condition, mutable_scope, capture_context)) : (empty_diagnostics()));
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(guard_diagnostics, walk_spawn_sites_in_expr(arm->body, mutable_scope, capture_context)));
}(); });
}
SpawnCaptureMutableWalkState apply_statement_to_mutable_scope(std::shared_ptr<ast::Stmt> statement, mlc::Array<mlc::String> binding_scope, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{
  return [&]() -> SpawnCaptureMutableWalkState {
auto __match_subject = statement;
if (std::holds_alternative<ast::StmtLet>((*__match_subject))) {
const ast::StmtLet& stmtLet = std::get<ast::StmtLet>((*__match_subject));
auto [name, is_mut, type_expression, value, __4] = stmtLet; return [&]() {
auto binding_type = resolve_binding_type(type_expression, value, type_environment, registry);
type_environment.set(name, binding_type);
auto scope_after = ((is_mut && (!type_is_sync_safe_shared(binding_type, registry))) ? (names_add(binding_scope, name)) : (binding_scope));
return SpawnCaptureMutableWalkState{empty_diagnostics(), scope_after, type_environment};
}();
}
if (std::holds_alternative<ast::StmtLetPattern>((*__match_subject))) {
const ast::StmtLetPattern& stmtLetPattern = std::get<ast::StmtLetPattern>((*__match_subject));
auto [pattern, is_mut, type_expression, value, __4, __5, __6] = stmtLetPattern; return [&]() {
auto binding_type = resolve_binding_type(type_expression, value, type_environment, registry);
auto bindings = names::pattern_bindings(pattern);
auto index = 0;
while ((index < bindings.length())) {
  type_environment.set(bindings[index], binding_type);
  (index = mlc::arith::checked_add(index, 1));
}
auto scope_after = ((is_mut && (!type_is_sync_safe_shared(binding_type, registry))) ? (names_add_all(binding_scope, bindings)) : (binding_scope));
return SpawnCaptureMutableWalkState{empty_diagnostics(), scope_after, type_environment};
}();
}
if (std::holds_alternative<ast::StmtLetConst>((*__match_subject))) {
const ast::StmtLetConst& stmtLetConst = std::get<ast::StmtLetConst>((*__match_subject));
auto [name, type_expression, value, __3] = stmtLetConst; return [&]() {
auto binding_type = resolve_binding_type(type_expression, value, type_environment, registry);
type_environment.set(name, binding_type);
return SpawnCaptureMutableWalkState{empty_diagnostics(), binding_scope, type_environment};
}();
}
return SpawnCaptureMutableWalkState{empty_diagnostics(), binding_scope, type_environment};
std::abort();
}();
}
SpawnCaptureMutableWalkState walk_spawn_sites_in_stmts_state(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return statements.fold(SpawnCaptureMutableWalkState{empty_diagnostics(), mutable_scope, capture_context.type_environment}, [=](SpawnCaptureMutableWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return [&]() {
auto nested_context = SpawnCaptureContext{capture_context.registry, state.type_environment};
auto value_diagnostics = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return walk_spawn_sites_in_expr(value, state.binding_scope, nested_context); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return ast::diagnostics_append(walk_spawn_sites_in_expr(value, state.binding_scope, nested_context), walk_spawn_sites_in_expr(else_expression, state.binding_scope, nested_context)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return walk_spawn_sites_in_expr(value, state.binding_scope, nested_context); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return walk_spawn_sites_in_expr(expression, state.binding_scope, nested_context); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return empty_diagnostics(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return empty_diagnostics(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return walk_spawn_sites_in_expr(expression, state.binding_scope, nested_context); }
}, (*statement));
auto after = apply_statement_to_mutable_scope(statement, state.binding_scope, state.type_environment, capture_context.registry);
return SpawnCaptureMutableWalkState{ast::diagnostics_append(state.diagnostics, value_diagnostics), after.binding_scope, after.type_environment};
}(); });
}
mlc::Array<ast::Diagnostic> walk_spawn_sites_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_scope, SpawnCaptureContext capture_context) noexcept{
  return walk_spawn_sites_in_stmts_state(statements, mutable_scope, capture_context).diagnostics;
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, bool value_discarded) noexcept{
  return expressions.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::Expr> expression) mutable { return ast::diagnostics_append(diagnostics, bare_spawn_diagnostics_in_expr(expression, value_discarded)); });
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, bool value_discarded) noexcept{
  return field_values.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::FieldVal> field_value) mutable { return ast::diagnostics_append(diagnostics, bare_spawn_diagnostics_in_expr(field_value->value, value_discarded)); });
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_record_parts(mlc::Array<ast::RecordLitPart> parts, bool value_discarded) noexcept{
  return parts.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) { auto [field_values] = recordLitFields; return ast::diagnostics_append(diagnostics, bare_spawn_diagnostics_in_field_values(field_values, value_discarded)); },
[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression] = recordLitSpread; return ast::diagnostics_append(diagnostics, bare_spawn_diagnostics_in_expr(spread_expression, value_discarded)); }
}, part); });
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, bool value_discarded) noexcept{
  return arms.fold(empty_diagnostics(), [=](mlc::Array<ast::Diagnostic> diagnostics, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto guard_diagnostics = (arm->has_guard ? (bare_spawn_diagnostics_in_expr(arm->when_condition, false)) : (empty_diagnostics()));
return ast::diagnostics_append(diagnostics, ast::diagnostics_append(guard_diagnostics, bare_spawn_diagnostics_in_expr(arm->body, value_discarded)));
}(); });
}
mlc::Array<ast::Diagnostic> bare_spawn_statement_diagnostics(std::shared_ptr<ast::Stmt> statement, bool statement_value_discarded) noexcept{
  return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [__0, __1, __2, value, __4] = stmtLet; return bare_spawn_diagnostics_in_expr(value, false); },
[&](const ast::StmtLetPattern& stmtLetPattern) { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(value, false), bare_spawn_diagnostics_in_expr(else_expression, false)); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [__0, __1, value, __3] = stmtLetConst; return bare_spawn_diagnostics_in_expr(value, false); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return bare_spawn_diagnostics_in_expr(expression, statement_value_discarded); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return empty_diagnostics(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return empty_diagnostics(); },
[&](const ast::StmtReturn& stmtReturn) { auto [expression, __1] = stmtReturn; return bare_spawn_diagnostics_in_expr(expression, false); }
}, (*statement));
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, bool last_provides_value) noexcept{
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  auto index = 0;
  auto count = statements.length();
  while ((index < count))   {
    auto is_last = (index == mlc::arith::checked_sub(count, 1));
    auto statement_value_discarded = (!(last_provides_value && is_last));
    (diagnostics = ast::diagnostics_append(diagnostics, bare_spawn_statement_diagnostics(statements[index], statement_value_discarded)));
    (index = mlc::arith::checked_add(index, 1));
  }
  return diagnostics;
}
mlc::Array<ast::Diagnostic> bare_spawn_diagnostics_in_expr(std::shared_ptr<ast::Expr> expression, bool value_discarded) noexcept{
  return std::visit(overloaded{[&](const ast::ExprSpawn& exprSpawn) { auto [body, source_span] = exprSpawn; return [&]() {
auto self_diagnostics = [&]() -> mlc::Array<ast::Diagnostic> {
  if (value_discarded)   {
    return [&]() {
auto diagnostics = mlc::Array<ast::Diagnostic>{};
diagnostics.push_back(bare_spawn_diagnostic(source_span));
return diagnostics;
}();
  } else   {
    return empty_diagnostics();
  }
}();
return ast::diagnostics_append(self_diagnostics, bare_spawn_diagnostics_in_stmts(body, true));
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
[&](const ast::ExprExtern& exprExtern) { auto [__0, __1, __2, __3] = exprExtern; return empty_diagnostics(); },
[&](const ast::ExprIdent& exprIdent) { auto [__0, __1] = exprIdent; return empty_diagnostics(); },
[&](const ast::ExprBin& exprBin) { auto [__0, left, right, __3] = exprBin; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(left, false), bare_spawn_diagnostics_in_expr(right, false)); },
[&](const ast::ExprUn& exprUn) { auto [__0, operand, __2] = exprUn; return bare_spawn_diagnostics_in_expr(operand, false); },
[&](const ast::ExprCall& exprCall) { auto [callee, arguments, __2] = exprCall; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(callee, false), bare_spawn_diagnostics_in_expr_list(arguments, false)); },
[&](const ast::ExprMethod& exprMethod) { auto [receiver, __1, arguments, __3] = exprMethod; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(receiver, false), bare_spawn_diagnostics_in_expr_list(arguments, false)); },
[&](const ast::ExprField& exprField) { auto [object, __1, __2] = exprField; return bare_spawn_diagnostics_in_expr(object, false); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, __2] = exprIndex; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(object, false), bare_spawn_diagnostics_in_expr(index_expression, false)); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_branch, else_branch, __3] = exprIf; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(condition, false), ast::diagnostics_append(bare_spawn_diagnostics_in_expr(then_branch, value_discarded), bare_spawn_diagnostics_in_expr(else_branch, value_discarded))); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result_expression, __2] = exprBlock; return ast::diagnostics_append(bare_spawn_diagnostics_in_stmts(statements, false), bare_spawn_diagnostics_in_expr(result_expression, value_discarded)); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, body, __2] = exprWhile; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(condition, false), bare_spawn_diagnostics_in_stmts(body, false)); },
[&](const ast::ExprFor& exprFor) { auto [__0, iterable, body, __3] = exprFor; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(iterable, false), bare_spawn_diagnostics_in_stmts(body, false)); },
[&](const ast::ExprMatch& exprMatch) { auto [scrutinee, arms, __2] = exprMatch; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(scrutinee, false), bare_spawn_diagnostics_in_match_arms(arms, value_discarded)); },
[&](const ast::ExprRecord& exprRecord) { auto [__0, parts, __2] = exprRecord; return bare_spawn_diagnostics_in_record_parts(parts, false); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_values, __3] = exprRecordUpdate; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(base, false), bare_spawn_diagnostics_in_field_values(field_values, false)); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return bare_spawn_diagnostics_in_expr_list(elements, false); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return bare_spawn_diagnostics_in_expr_list(elements, false); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, __1] = exprQuestion; return bare_spawn_diagnostics_in_expr(inner, false); },
[&](const ast::ExprLambda& exprLambda) { auto [__0, body, __2] = exprLambda; return bare_spawn_diagnostics_in_expr(body, false); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, value, __2] = exprNamedArg; return bare_spawn_diagnostics_in_expr(value, false); },
[&](const ast::ExprWith& exprWith) { auto [resource, __1, body, __3] = exprWith; return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(resource, false), bare_spawn_diagnostics_in_stmts(body, false)); },
[&](const ast::ExprScope& exprScope) { auto [__0, body, __2] = exprScope; return bare_spawn_diagnostics_in_stmts(body, false); },
[&](const ast::ExprRegion& exprRegion) { auto [__0, body, __2] = exprRegion; return bare_spawn_diagnostics_in_stmts(body, false); }
}, (*expression));
}
mlc::Array<ast::Diagnostic> spawn_mutable_capture_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body, registry::TypeRegistry registry) noexcept{
  auto capture_context = SpawnCaptureContext{registry, type_environment_from_parameters(parameters, registry)};
  return ast::diagnostics_append(bare_spawn_diagnostics_in_expr(body, false), walk_spawn_sites_in_expr(body, mutable_scope_from_parameters(parameters, registry), capture_context));
}

} // namespace spawn_capture
