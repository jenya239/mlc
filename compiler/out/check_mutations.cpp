#include "check_mutations.hpp"

#include "ast.hpp"
#include "names.hpp"

namespace check_mutations {

using namespace ast;
using namespace names;
using namespace ast_tokens;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_for_record_literal_single_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept{return scope.any([sought_name](mlc::String scope_entry) mutable { return scope_entry == sought_name; });}

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
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner, _w1] = _v_exprun; return check_mutation_expr(inner, mutable_locals); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, arguments, _w0] = _v_exprcall; return check_mutation_call(function, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, _w0, arguments, _w1] = _v_exprmethod; return check_mutation_call(object, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return check_mutation_expr(object, mutable_locals); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index, _w0] = _v_exprindex; return ast::diagnostics_append(check_mutation_expr(object, mutable_locals), check_mutation_expr(index, mutable_locals)); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expr, else_expr, _w0] = _v_exprif; return check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>>{condition, then_expr, else_expr}, mutable_locals); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return check_mutation_block(statements, result, mutable_locals); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return ast::diagnostics_append(check_mutation_expr(condition, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [_w0, iterator, statements, _w1] = _v_exprfor; return ast::diagnostics_append(check_mutation_expr(iterator, mutable_locals), check_mutation_stmts_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, arms, _w0] = _v_exprmatch; return ast::diagnostics_append(check_mutation_expr(subject, mutable_locals), check_mutation_arms(arms, mutable_locals)); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, lit_parts, _w1] = _v_exprrecord; return check_mutation_record_lit_parts(lit_parts, mutable_locals); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, field_values, _w1] = _v_exprrecordupdate; return ast::diagnostics_append(check_mutation_expr(base, mutable_locals), check_mutation_field_vals(field_values, mutable_locals)); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner, _w0] = _v_exprquestion; return check_mutation_expr(inner, mutable_locals); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [_w0, body, _w1] = _v_exprlambda; return check_mutation_expr(body, [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }()); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner, _w1] = _v_exprnamedarg; return check_mutation_expr(inner, mutable_locals); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, _w0, stmts, _w1] = _v_exprwith; return ast::diagnostics_append(check_mutation_expr(resource, mutable_locals), check_mutation_stmts_errors(stmts, mutable_locals)); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(); }();}

mlc::Array<ast::Diagnostic> diagnostics_for_record_literal_single_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> mutable_locals) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<ast::Diagnostic> { auto [field_values] = recordlitfields; return check_mutation_field_vals(field_values, mutable_locals); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<ast::Diagnostic> { auto [spread_expression] = recordlitspread; return check_mutation_expr(spread_expression, mutable_locals); }
}, literal_part._);}

mlc::Array<ast::Diagnostic> check_mutation_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> mutable_locals) noexcept{return mlc::collections::flat_map(lit_parts, [mutable_locals](ast::RecordLitPart literal_part_under_check) mutable { return diagnostics_for_record_literal_single_part(literal_part_under_check, mutable_locals); });}

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors_from_callable = check_mutation_expr(function, mutable_locals);
return arguments.fold(errors_from_callable, [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> argument_under_call) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(argument_under_call, mutable_locals)); });
}

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept{return expressions.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> expression_under_list) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression_under_list, mutable_locals)); });}

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept{return field_values.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(field_value_under_walk->val, mutable_locals)); });}

mlc::Array<ast::Diagnostic> check_mutation_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, mlc::Array<mlc::String> mutable_locals) noexcept{return arms.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty = {};
  return empty;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::MatchArm> arm_under_walk) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> bindings = names::pattern_bindings(arm_under_walk->pat);
  mlc::Array<mlc::String> arm_scope = mutable_locals;
  int binding_index = 0;
  while (binding_index < bindings.size()){
{
arm_scope.push_back(bindings[binding_index]);
binding_index = binding_index + 1;
}
}
  mlc::Array<ast::Diagnostic> after_guard = arm_under_walk->has_guard ? ast::diagnostics_append(errors_accumulator, check_mutation_expr(arm_under_walk->when_condition, arm_scope)) : errors_accumulator;
  return ast::diagnostics_append(after_guard, check_mutation_expr(arm_under_walk->body, arm_scope));
 }(); });}

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<mlc::String> inner_mutable = mutable_locals;
mlc::Array<ast::Diagnostic> statement_errors = statements.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> starting_errors = {};
  return starting_errors;
 }(), [&inner_mutable](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Stmt> statement_under_block) mutable { return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> mlc::Array<ast::Diagnostic> { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
inner_mutable.push_back(name);
}
}
  return next_errors;
 }(); },
  [&](const StmtLetPat& stmtletpat) -> mlc::Array<ast::Diagnostic> { auto [pattern, is_mut, _w0, value, _w1, _w2, _w3] = stmtletpat; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
mlc::Array<mlc::String> bound_pattern_names = names::pattern_bindings(pattern);
int pattern_binding_index = 0;
[&]() { 
  while (pattern_binding_index < bound_pattern_names.size()){
{
inner_mutable.push_back(bound_pattern_names[pattern_binding_index]);
pattern_binding_index = pattern_binding_index + 1;
}
}
 }();
}
}
  return next_errors;
 }(); },
  [&](const StmtLetConst& stmtletconst) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1, value, _w2] = stmtletconst; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable)); },
  [&](const StmtExpr& stmtexpr) -> mlc::Array<ast::Diagnostic> { auto [expression, _w0] = stmtexpr; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
  [&](const StmtReturn& stmtreturn) -> mlc::Array<ast::Diagnostic> { auto [expression, _w0] = stmtreturn; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
  [&](const StmtBreak& stmtbreak) -> mlc::Array<ast::Diagnostic> { auto [_w0] = stmtbreak; return errors_accumulator; },
  [&](const StmtContinue& stmtcontinue) -> mlc::Array<ast::Diagnostic> { auto [_w0] = stmtcontinue; return errors_accumulator; }
}, (*statement_under_block)._); });
return ast::diagnostics_append(statement_errors, check_mutation_expr(result, inner_mutable));
}

mlc::Array<ast::Diagnostic> check_mutation_stmts_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<mlc::String> inner_mutable = mutable_locals;
return statements.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> starting_errors = {};
  return starting_errors;
 }(), [&inner_mutable](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> mlc::Array<ast::Diagnostic> { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
inner_mutable.push_back(name);
}
}
  return next_errors;
 }(); },
  [&](const StmtLetPat& stmtletpat) -> mlc::Array<ast::Diagnostic> { auto [pattern, is_mut, _w0, value, _w1, _w2, _w3] = stmtletpat; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> next_errors = ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable));
  if (is_mut){
{
mlc::Array<mlc::String> bound_pattern_names = names::pattern_bindings(pattern);
int pattern_binding_index = 0;
[&]() { 
  while (pattern_binding_index < bound_pattern_names.size()){
{
inner_mutable.push_back(bound_pattern_names[pattern_binding_index]);
pattern_binding_index = pattern_binding_index + 1;
}
}
 }();
}
}
  return next_errors;
 }(); },
  [&](const StmtLetConst& stmtletconst) -> mlc::Array<ast::Diagnostic> { auto [_w0, _w1, value, _w2] = stmtletconst; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(value, inner_mutable)); },
  [&](const StmtExpr& stmtexpr) -> mlc::Array<ast::Diagnostic> { auto [expression, _w0] = stmtexpr; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
  [&](const StmtReturn& stmtreturn) -> mlc::Array<ast::Diagnostic> { auto [expression, _w0] = stmtreturn; return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression, inner_mutable)); },
  [&](const StmtBreak& stmtbreak) -> mlc::Array<ast::Diagnostic> { auto [_w0] = stmtbreak; return errors_accumulator; },
  [&](const StmtContinue& stmtcontinue) -> mlc::Array<ast::Diagnostic> { auto [_w0] = stmtcontinue; return errors_accumulator; }
}, (*statement_under_walk)._); });
}

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<ast::Expr> body) noexcept{
mlc::Array<mlc::String> mutable_locals = params.fold([&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }(), [](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return [&]() -> mlc::Array<mlc::String> { 
  if (parameter_under_check->is_mut){
{
parameter_scope.push_back(parameter_under_check->name);
}
}
  return parameter_scope;
 }(); });
return check_mutation_expr(body, mutable_locals);
}

} // namespace check_mutations
