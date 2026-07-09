#define main mlc_user_main
#include "move_check.hpp"

#include "ast.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"

namespace move_check {

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
mlc::Array<mlc::String> names_remove(mlc::Array<mlc::String> names, mlc::String name) noexcept{
  return names.filter([=](mlc::String entry) mutable { return (entry != name); });
}
mlc::Array<mlc::String> names_remove_all(mlc::Array<mlc::String> names, mlc::Array<mlc::String> removals) noexcept{
  return removals.fold(names, [=](mlc::Array<mlc::String> accumulated, mlc::String removal) mutable { return names_remove(accumulated, removal); });
}
mlc::Array<ast::Diagnostic> empty_diagnostics() noexcept{
  auto diagnostics = mlc::Array<ast::Diagnostic>{};
  return diagnostics;
}
ast::Diagnostic use_after_move_diagnostic(mlc::String binding_name, ast::Span source_span) noexcept{
  return ast::diagnostic_error_with_code(((mlc::String("value '", 7) + binding_name) + mlc::String("' was moved and cannot be used here", 35)), source_span, diagnostic_codes::diagnostic_code_e088());
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
MoveCheckWalkState move_check_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> moved) noexcept{
  return std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) -> MoveCheckWalkState { auto [name, source_span] = exprIdent; return (names_contains(moved, name) ? (MoveCheckWalkState{mlc::Array<ast::Diagnostic>{use_after_move_diagnostic(name, source_span)}, moved}) : (MoveCheckWalkState{empty_diagnostics(), moved})); },
[&](const ast::ExprUn& exprUn) -> MoveCheckWalkState { auto [operation, operand, __2] = exprUn; return [&]() -> MoveCheckWalkState {
  if ((operation == mlc::String("move", 4)))   {
    return [&]() -> MoveCheckWalkState {
auto __match_subject = operand;
if (std::holds_alternative<ast::ExprIdent>((*__match_subject))) {
const ast::ExprIdent& exprIdent = std::get<ast::ExprIdent>((*__match_subject));
auto [name, __1] = exprIdent; return MoveCheckWalkState{empty_diagnostics(), names_add(moved, name)};
}
return move_check_expr(operand, moved);
std::abort();
}();
  } else   {
    return move_check_expr(operand, moved);
  }
}(); },
[&](const ast::ExprInt& exprInt) -> MoveCheckWalkState { auto [__0, __1] = exprInt; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprStr& exprStr) -> MoveCheckWalkState { auto [__0, __1] = exprStr; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprBool& exprBool) -> MoveCheckWalkState { auto [__0, __1] = exprBool; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprUnit& exprUnit) -> MoveCheckWalkState { auto [__0] = exprUnit; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprFloat& exprFloat) -> MoveCheckWalkState { auto [__0, __1] = exprFloat; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprI64& exprI64) -> MoveCheckWalkState { auto [__0, __1] = exprI64; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprU8& exprU8) -> MoveCheckWalkState { auto [__0, __1] = exprU8; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprUsize& exprUsize) -> MoveCheckWalkState { auto [__0, __1] = exprUsize; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprChar& exprChar) -> MoveCheckWalkState { auto [__0, __1] = exprChar; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprExtern& exprExtern) -> MoveCheckWalkState { auto [__0] = exprExtern; return MoveCheckWalkState{empty_diagnostics(), moved}; },
[&](const ast::ExprBin& exprBin) -> MoveCheckWalkState { auto [__0, left, right, __3] = exprBin; return [&]() {
auto left_state = move_check_expr(left, moved);
auto right_state = move_check_expr(right, left_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(left_state.diagnostics, right_state.diagnostics), right_state.moved};
}(); },
[&](const ast::ExprCall& exprCall) -> MoveCheckWalkState { auto [callee, arguments, __2] = exprCall; return [&]() {
auto callee_state = move_check_expr(callee, moved);
return move_check_expr_list(arguments, callee_state);
}(); },
[&](const ast::ExprMethod& exprMethod) -> MoveCheckWalkState { auto [receiver, __1, arguments, __3] = exprMethod; return [&]() {
auto receiver_state = move_check_expr(receiver, moved);
return move_check_expr_list(arguments, receiver_state);
}(); },
[&](const ast::ExprField& exprField) -> MoveCheckWalkState { auto [object, __1, __2] = exprField; return move_check_expr(object, moved); },
[&](const ast::ExprIndex& exprIndex) -> MoveCheckWalkState { auto [object, index_expression, __2] = exprIndex; return [&]() {
auto object_state = move_check_expr(object, moved);
auto index_state = move_check_expr(index_expression, object_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(object_state.diagnostics, index_state.diagnostics), index_state.moved};
}(); },
[&](const ast::ExprIf& exprIf) -> MoveCheckWalkState { auto [condition, then_branch, else_branch, __3] = exprIf; return [&]() {
auto condition_state = move_check_expr(condition, moved);
auto then_state = move_check_expr(then_branch, condition_state.moved);
auto else_state = move_check_expr(else_branch, condition_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(condition_state.diagnostics, ast::diagnostics_append(then_state.diagnostics, else_state.diagnostics)), names_add_all(then_state.moved, else_state.moved)};
}(); },
[&](const ast::ExprBlock& exprBlock) -> MoveCheckWalkState { auto [statements, result_expression, __2] = exprBlock; return [&]() {
auto statements_state = move_check_stmts(statements, moved);
auto result_state = move_check_expr(result_expression, statements_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(statements_state.diagnostics, result_state.diagnostics), result_state.moved};
}(); },
[&](const ast::ExprWhile& exprWhile) -> MoveCheckWalkState { auto [condition, body, __2] = exprWhile; return [&]() {
auto condition_state = move_check_expr(condition, moved);
auto body_state = move_check_stmts(body, condition_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(condition_state.diagnostics, body_state.diagnostics), names_add_all(condition_state.moved, body_state.moved)};
}(); },
[&](const ast::ExprFor& exprFor) -> MoveCheckWalkState { auto [__0, iterable, body, __3] = exprFor; return [&]() {
auto iterable_state = move_check_expr(iterable, moved);
auto body_state = move_check_stmts(body, iterable_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(iterable_state.diagnostics, body_state.diagnostics), names_add_all(iterable_state.moved, body_state.moved)};
}(); },
[&](const ast::ExprMatch& exprMatch) -> MoveCheckWalkState { auto [scrutinee, arms, __2] = exprMatch; return [&]() {
auto scrutinee_state = move_check_expr(scrutinee, moved);
return move_check_match_arms(arms, scrutinee_state);
}(); },
[&](const ast::ExprRecord& exprRecord) -> MoveCheckWalkState { auto [__0, parts, __2] = exprRecord; return move_check_record_parts(parts, moved); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) -> MoveCheckWalkState { auto [__0, base, field_values, __3] = exprRecordUpdate; return [&]() {
auto base_state = move_check_expr(base, moved);
return move_check_field_values(field_values, base_state);
}(); },
[&](const ast::ExprArray& exprArray) -> MoveCheckWalkState { auto [elements, __1] = exprArray; return move_check_expr_list(elements, MoveCheckWalkState{empty_diagnostics(), moved}); },
[&](const ast::ExprTuple& exprTuple) -> MoveCheckWalkState { auto [elements, __1] = exprTuple; return move_check_expr_list(elements, MoveCheckWalkState{empty_diagnostics(), moved}); },
[&](const ast::ExprQuestion& exprQuestion) -> MoveCheckWalkState { auto [inner, __1] = exprQuestion; return move_check_expr(inner, moved); },
[&](const ast::ExprLambda& exprLambda) -> MoveCheckWalkState { auto [__0, body, __2] = exprLambda; return [&]() {
auto body_state = move_check_expr(body, empty_names());
return MoveCheckWalkState{body_state.diagnostics, moved};
}(); },
[&](const ast::ExprSpawn& exprSpawn) -> MoveCheckWalkState { auto [body, __1] = exprSpawn; return [&]() {
auto body_state = move_check_stmts(body, moved);
return MoveCheckWalkState{body_state.diagnostics, body_state.moved};
}(); },
[&](const ast::ExprNamedArg& exprNamedArg) -> MoveCheckWalkState { auto [__0, value, __2] = exprNamedArg; return move_check_expr(value, moved); },
[&](const ast::ExprWith& exprWith) -> MoveCheckWalkState { auto [resource, __1, body, __3] = exprWith; return [&]() {
auto resource_state = move_check_expr(resource, moved);
auto body_state = move_check_stmts(body, resource_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(resource_state.diagnostics, body_state.diagnostics), body_state.moved};
}(); }
}, (*expression));
}
MoveCheckWalkState move_check_expr_list(mlc::Array<std::shared_ptr<ast::Expr>> expressions, MoveCheckWalkState state) noexcept{
  return expressions.fold(state, [=](MoveCheckWalkState accumulator, std::shared_ptr<ast::Expr> expression) mutable { return [&]() {
auto next = move_check_expr(expression, accumulator.moved);
return MoveCheckWalkState{ast::diagnostics_append(accumulator.diagnostics, next.diagnostics), next.moved};
}(); });
}
MoveCheckWalkState move_check_field_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, MoveCheckWalkState state) noexcept{
  return field_values.fold(state, [=](MoveCheckWalkState accumulator, std::shared_ptr<ast::FieldVal> field_value) mutable { return [&]() {
auto next = move_check_expr(field_value->value, accumulator.moved);
return MoveCheckWalkState{ast::diagnostics_append(accumulator.diagnostics, next.diagnostics), next.moved};
}(); });
}
MoveCheckWalkState move_check_record_parts(mlc::Array<ast::RecordLitPart> parts, mlc::Array<mlc::String> moved) noexcept{
  return parts.fold(MoveCheckWalkState{empty_diagnostics(), moved}, [=](MoveCheckWalkState accumulator, ast::RecordLitPart part) mutable { return std::visit(overloaded{[&](const ast::RecordLitFields& recordLitFields) -> MoveCheckWalkState { auto [field_values] = recordLitFields; return move_check_field_values(field_values, accumulator); },
[&](const ast::RecordLitSpread& recordLitSpread) -> MoveCheckWalkState { auto [spread_expression] = recordLitSpread; return [&]() {
auto next = move_check_expr(spread_expression, accumulator.moved);
return MoveCheckWalkState{ast::diagnostics_append(accumulator.diagnostics, next.diagnostics), next.moved};
}(); }
}, part); });
}
MoveCheckWalkState move_check_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, MoveCheckWalkState state) noexcept{
  return arms.fold(MoveCheckWalkState{state.diagnostics, state.moved}, [=](MoveCheckWalkState accumulator, std::shared_ptr<ast::MatchArm> arm) mutable { return [&]() {
auto guard_state = (arm->has_guard ? (move_check_expr(arm->when_condition, state.moved)) : (MoveCheckWalkState{empty_diagnostics(), state.moved}));
auto body_state = move_check_expr(arm->body, guard_state.moved);
return MoveCheckWalkState{ast::diagnostics_append(accumulator.diagnostics, ast::diagnostics_append(guard_state.diagnostics, body_state.diagnostics)), names_add_all(accumulator.moved, body_state.moved)};
}(); });
}
MoveCheckWalkState move_check_stmts(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> moved) noexcept{
  return statements.fold(MoveCheckWalkState{empty_diagnostics(), moved}, [=](MoveCheckWalkState state, std::shared_ptr<ast::Stmt> statement) mutable { return [&]() {
auto value_state = std::visit(overloaded{[&](const ast::StmtLet& stmtLet) -> MoveCheckWalkState { auto [__0, __1, __2, value, __4] = stmtLet; return move_check_expr(value, state.moved); },
[&](const ast::StmtLetPattern& stmtLetPattern) -> MoveCheckWalkState { auto [__0, __1, __2, value, __4, else_expression, __6] = stmtLetPattern; return [&]() {
auto value_checked = move_check_expr(value, state.moved);
auto else_checked = move_check_expr(else_expression, value_checked.moved);
return MoveCheckWalkState{ast::diagnostics_append(value_checked.diagnostics, else_checked.diagnostics), else_checked.moved};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) -> MoveCheckWalkState { auto [__0, __1, value, __3] = stmtLetConst; return move_check_expr(value, state.moved); },
[&](const ast::StmtExpr& stmtExpr) -> MoveCheckWalkState { auto [expression, __1] = stmtExpr; return move_check_expr(expression, state.moved); },
[&](const ast::StmtBreak& stmtBreak) -> MoveCheckWalkState { auto [__0] = stmtBreak; return MoveCheckWalkState{empty_diagnostics(), state.moved}; },
[&](const ast::StmtContinue& stmtContinue) -> MoveCheckWalkState { auto [__0] = stmtContinue; return MoveCheckWalkState{empty_diagnostics(), state.moved}; },
[&](const ast::StmtReturn& stmtReturn) -> MoveCheckWalkState { auto [expression, __1] = stmtReturn; return move_check_expr(expression, state.moved); }
}, (*statement));
auto bindings = statement_binding_names(statement);
return MoveCheckWalkState{ast::diagnostics_append(state.diagnostics, value_state.diagnostics), names_remove_all(value_state.moved, bindings)};
}(); });
}
mlc::Array<ast::Diagnostic> move_use_after_diagnostics(mlc::Array<std::shared_ptr<ast::Param>> _parameters, std::shared_ptr<ast::Expr> body) noexcept{
  return move_check_expr(body, empty_names()).diagnostics;
}

} // namespace move_check
