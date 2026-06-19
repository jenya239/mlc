#include "check_mutations.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"

namespace check_mutations {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace names;
using namespace diagnostic_codes;
using namespace ast_tokens;

struct MutationsPass {mlc::Array<mlc::String> mutable_locals;int seed;};

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept;

mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> scope, mlc::String binding_name) noexcept;

mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept;

mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> scope, mlc::String name, bool is_mut) noexcept;

mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> diagnostics_for_record_literal_single_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_statements_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept;

check_mutations::MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept;

check_mutations::MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept;

check_mutations::MutationCheckResult merge_mutation_check_results(check_mutations::MutationCheckResult first, check_mutations::MutationCheckResult second) noexcept;

check_mutations::MutationCheckResult dispatch_mutations_pass(check_mutations::MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, check_mutations::MutationsPass mutations_pass) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, check_mutations::MutationsPass mutations_pass) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_int(check_mutations::MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_str(check_mutations::MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_bool(check_mutations::MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_unit(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_float(check_mutations::MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_i64(check_mutations::MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_u8(check_mutations::MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_usize(check_mutations::MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_char(check_mutations::MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_extern(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_ident(check_mutations::MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_bin(check_mutations::MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_un(check_mutations::MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_call(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_method(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_field(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_index(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_if(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_block(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_while(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_for(check_mutations::MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_match(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_record(check_mutations::MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_record_update(check_mutations::MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_array(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_tuple(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_question(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_lambda(check_mutations::MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_with(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_unsupported(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept{return scope.any([sought_name](mlc::String scope_entry) mutable { return scope_entry == sought_name; });}

mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> scope, mlc::String binding_name) noexcept{return scope.concat(mlc::Array<mlc::String>{binding_name});}

mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{return binding_names.fold(scope, [](mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) mutable { return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name}); });}

mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> scope, mlc::String name, bool is_mut) noexcept{return is_mut ? mutable_scope_with_binding(scope, name) : scope;}

mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept{return is_mut ? mutable_scope_with_bindings(scope, names::pattern_bindings(pattern)) : scope;}

mlc::Array<ast::Diagnostic> check_mutation_expr(std::shared_ptr<ast::Expr> expression, mlc::Array<mlc::String> mutable_locals) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprBin>((*expression)._)) { auto _v_exprbin = std::get<ast::ExprBin>((*expression)._); auto [operation, left, right, source_span] = _v_exprbin; return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> right_errors = check_mutation_expr(right, mutable_locals);
  return operation == mlc::String("=") ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> assignment_errors = [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::ExprIdent>((*left)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*left)._); auto [name, span] = _v_exprident; return !scope_has(mutable_locals, name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("cannot assign to immutable binding: ") + name, span, diagnostic_codes::diagnostic_code_e031())} : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(); }();
  return ast::diagnostics_append(assignment_errors, right_errors);
 }() : ast::diagnostics_append(check_mutation_expr(left, mutable_locals), right_errors);
 }(); } if (std::holds_alternative<ast::ExprUn>((*expression)._)) { auto _v_exprun = std::get<ast::ExprUn>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprun; return check_mutation_expr(inner_expression, mutable_locals); } if (std::holds_alternative<ast::ExprCall>((*expression)._)) { auto _v_exprcall = std::get<ast::ExprCall>((*expression)._); auto [function, arguments, _w0] = _v_exprcall; return check_mutation_call(function, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprMethod>((*expression)._)) { auto _v_exprmethod = std::get<ast::ExprMethod>((*expression)._); auto [object, _w0, arguments, _w1] = _v_exprmethod; return check_mutation_call(object, arguments, mutable_locals); } if (std::holds_alternative<ast::ExprField>((*expression)._)) { auto _v_exprfield = std::get<ast::ExprField>((*expression)._); auto [object, _w0, _w1] = _v_exprfield; return check_mutation_expr(object, mutable_locals); } if (std::holds_alternative<ast::ExprIndex>((*expression)._)) { auto _v_exprindex = std::get<ast::ExprIndex>((*expression)._); auto [object, index_expression, _w0] = _v_exprindex; return ast::diagnostics_append(check_mutation_expr(object, mutable_locals), check_mutation_expr(index_expression, mutable_locals)); } if (std::holds_alternative<ast::ExprIf>((*expression)._)) { auto _v_exprif = std::get<ast::ExprIf>((*expression)._); auto [condition, then_expression, else_expression, _w0] = _v_exprif; return check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>>{condition, then_expression, else_expression}, mutable_locals); } if (std::holds_alternative<ast::ExprBlock>((*expression)._)) { auto _v_exprblock = std::get<ast::ExprBlock>((*expression)._); auto [statements, result, _w0] = _v_exprblock; return check_mutation_block(statements, result, mutable_locals); } if (std::holds_alternative<ast::ExprWhile>((*expression)._)) { auto _v_exprwhile = std::get<ast::ExprWhile>((*expression)._); auto [condition, statements, _w0] = _v_exprwhile; return ast::diagnostics_append(check_mutation_expr(condition, mutable_locals), check_mutation_statements_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprFor>((*expression)._)) { auto _v_exprfor = std::get<ast::ExprFor>((*expression)._); auto [_w0, iterator, statements, _w1] = _v_exprfor; return ast::diagnostics_append(check_mutation_expr(iterator, mutable_locals), check_mutation_statements_errors(statements, mutable_locals)); } if (std::holds_alternative<ast::ExprMatch>((*expression)._)) { auto _v_exprmatch = std::get<ast::ExprMatch>((*expression)._); auto [subject, match_arms, _w0] = _v_exprmatch; return ast::diagnostics_append(check_mutation_expr(subject, mutable_locals), check_mutation_match_arms(match_arms, mutable_locals)); } if (std::holds_alternative<ast::ExprRecord>((*expression)._)) { auto _v_exprrecord = std::get<ast::ExprRecord>((*expression)._); auto [_w0, lit_parts, _w1] = _v_exprrecord; return check_mutation_record_lit_parts(lit_parts, mutable_locals); } if (std::holds_alternative<ast::ExprRecordUpdate>((*expression)._)) { auto _v_exprrecordupdate = std::get<ast::ExprRecordUpdate>((*expression)._); auto [_w0, base, field_values, _w1] = _v_exprrecordupdate; return ast::diagnostics_append(check_mutation_expr(base, mutable_locals), check_mutation_field_vals(field_values, mutable_locals)); } if (std::holds_alternative<ast::ExprArray>((*expression)._)) { auto _v_exprarray = std::get<ast::ExprArray>((*expression)._); auto [elements, _w0] = _v_exprarray; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprTuple>((*expression)._)) { auto _v_exprtuple = std::get<ast::ExprTuple>((*expression)._); auto [elements, _w0] = _v_exprtuple; return check_mutation_exprs(elements, mutable_locals); } if (std::holds_alternative<ast::ExprQuestion>((*expression)._)) { auto _v_exprquestion = std::get<ast::ExprQuestion>((*expression)._); auto [inner_expression, _w0] = _v_exprquestion; return check_mutation_expr(inner_expression, mutable_locals); } if (std::holds_alternative<ast::ExprLambda>((*expression)._)) { auto _v_exprlambda = std::get<ast::ExprLambda>((*expression)._); auto [_w0, body, _w1] = _v_exprlambda; return check_mutation_expr(body, [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }()); } if (std::holds_alternative<ast::ExprNamedArg>((*expression)._)) { auto _v_exprnamedarg = std::get<ast::ExprNamedArg>((*expression)._); auto [_w0, inner_expression, _w1] = _v_exprnamedarg; return check_mutation_expr(inner_expression, mutable_locals); } if (std::holds_alternative<ast::ExprWith>((*expression)._)) { auto _v_exprwith = std::get<ast::ExprWith>((*expression)._); auto [resource, _w0, statements, _w1] = _v_exprwith; return ast::diagnostics_append(check_mutation_expr(resource, mutable_locals), check_mutation_statements_errors(statements, mutable_locals)); } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(); }();}

mlc::Array<ast::Diagnostic> diagnostics_for_record_literal_single_part(ast::RecordLitPart literal_part, mlc::Array<mlc::String> mutable_locals) noexcept{return std::visit(overloaded{
  [&](const RecordLitFields& recordlitfields) -> mlc::Array<ast::Diagnostic> { auto [field_values] = recordlitfields; return check_mutation_field_vals(field_values, mutable_locals); },
  [&](const RecordLitSpread& recordlitspread) -> mlc::Array<ast::Diagnostic> { auto [spread_expression] = recordlitspread; return check_mutation_expr(spread_expression, mutable_locals); }
}, literal_part);}

mlc::Array<ast::Diagnostic> check_mutation_record_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts, mlc::Array<mlc::String> mutable_locals) noexcept{return mlc::collections::flat_map(lit_parts, [mutable_locals](ast::RecordLitPart literal_part_under_check) mutable { return diagnostics_for_record_literal_single_part(literal_part_under_check, mutable_locals); });}

mlc::Array<ast::Diagnostic> check_mutation_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> mutable_locals) noexcept{
mlc::Array<ast::Diagnostic> errors_from_callable = check_mutation_expr(function, mutable_locals);
return arguments.fold(errors_from_callable, [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> argument_under_call) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(argument_under_call, mutable_locals)); });
}

mlc::Array<ast::Diagnostic> check_mutation_exprs(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::Array<mlc::String> mutable_locals) noexcept{return expressions.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::Expr> expression_under_list) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(expression_under_list, mutable_locals)); });}

mlc::Array<ast::Diagnostic> check_mutation_field_vals(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::Array<mlc::String> mutable_locals) noexcept{return field_values.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::FieldVal> field_value_under_walk) mutable { return ast::diagnostics_append(errors_accumulator, check_mutation_expr(field_value_under_walk->value, mutable_locals)); });}

mlc::Array<ast::Diagnostic> check_mutation_match_arms(mlc::Array<std::shared_ptr<ast::MatchArm>> match_arms, mlc::Array<mlc::String> mutable_locals) noexcept{return match_arms.fold([&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_diagnostics = {};
  return empty_diagnostics;
 }(), [mutable_locals](mlc::Array<ast::Diagnostic> errors_accumulator, std::shared_ptr<ast::MatchArm> arm_under_walk) mutable { return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<mlc::String> arm_scope = mutable_scope_with_bindings(mutable_locals, names::pattern_bindings(arm_under_walk->pattern));
  mlc::Array<ast::Diagnostic> after_guard = arm_under_walk->has_guard ? ast::diagnostics_append(errors_accumulator, check_mutation_expr(arm_under_walk->when_condition, arm_scope)) : errors_accumulator;
  return ast::diagnostics_append(after_guard, check_mutation_expr(arm_under_walk->body, arm_scope));
 }(); });}

mlc::Array<ast::Diagnostic> check_mutation_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, mlc::Array<mlc::String> mutable_locals) noexcept{
check_mutations::MutationCheckResult block_check = statements.fold(check_mutations::MutationCheckResult{{}, mutable_locals}, [](check_mutations::MutationCheckResult accumulator, std::shared_ptr<ast::Stmt> statement_under_block) mutable { return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> check_mutations::MutationCheckResult { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after = mutable_scope_after_let(accumulator.mutable_locals, name, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, accumulator.mutable_locals)), scope_after};
 }(); },
  [&](const StmtLetPattern& stmtletpattern) -> check_mutations::MutationCheckResult { auto [pattern, is_mut, _w0, value, _w1, _w2, _w3] = stmtletpattern; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.mutable_locals;
  mlc::Array<mlc::String> scope_after_bindings = mutable_scope_after_pattern_let(scope_after_value, pattern, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, scope_after_value)), scope_after_bindings};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> check_mutations::MutationCheckResult { auto [_w0, _w1, value, _w2] = stmtletconst; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtExpr& stmtexpr) -> check_mutations::MutationCheckResult { auto [expression, _w0] = stmtexpr; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtReturn& stmtreturn) -> check_mutations::MutationCheckResult { auto [expression, _w0] = stmtreturn; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtBreak& stmtbreak) -> check_mutations::MutationCheckResult { auto [_w0] = stmtbreak; return accumulator; },
  [&](const StmtContinue& stmtcontinue) -> check_mutations::MutationCheckResult { auto [_w0] = stmtcontinue; return accumulator; }
}, (*statement_under_block)._); });
return ast::diagnostics_append(block_check.diagnostics, check_mutation_expr(result_expression, block_check.mutable_locals));
}

mlc::Array<ast::Diagnostic> check_mutation_statements_errors(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{
check_mutations::MutationCheckResult fold_result = statements.fold(check_mutations::MutationCheckResult{{}, mutable_locals}, [](check_mutations::MutationCheckResult accumulator, std::shared_ptr<ast::Stmt> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const StmtLet& stmtlet) -> check_mutations::MutationCheckResult { auto [name, is_mut, _w0, value, _w1] = stmtlet; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after = mutable_scope_after_let(accumulator.mutable_locals, name, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, accumulator.mutable_locals)), scope_after};
 }(); },
  [&](const StmtLetPattern& stmtletpattern) -> check_mutations::MutationCheckResult { auto [pattern, is_mut, _w0, value, _w1, _w2, _w3] = stmtletpattern; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.mutable_locals;
  mlc::Array<mlc::String> scope_after_bindings = mutable_scope_after_pattern_let(scope_after_value, pattern, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, scope_after_value)), scope_after_bindings};
 }(); },
  [&](const StmtLetConst& stmtletconst) -> check_mutations::MutationCheckResult { auto [_w0, _w1, value, _w2] = stmtletconst; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(value, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtExpr& stmtexpr) -> check_mutations::MutationCheckResult { auto [expression, _w0] = stmtexpr; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtReturn& stmtreturn) -> check_mutations::MutationCheckResult { auto [expression, _w0] = stmtreturn; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_expr(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const StmtBreak& stmtbreak) -> check_mutations::MutationCheckResult { auto [_w0] = stmtbreak; return accumulator; },
  [&](const StmtContinue& stmtcontinue) -> check_mutations::MutationCheckResult { auto [_w0] = stmtcontinue; return accumulator; }
}, (*statement_under_walk)._); });
return fold_result.diagnostics;
}

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept{
mlc::Array<mlc::String> mutable_locals = parameters.fold([&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }(), [](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return mutable_scope_after_let(parameter_scope, parameter_under_check->name, parameter_under_check->is_mut); });
return check_mutation_expr(body, mutable_locals);
}

check_mutations::MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::MutationsPass{mutable_locals, 0};}

mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept{
mlc::Array<ast::Diagnostic> empty_diagnostics = {};
return empty_diagnostics;
}

check_mutations::MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::MutationCheckResult{mutation_check_empty(), mutable_locals};}

check_mutations::MutationCheckResult merge_mutation_check_results(check_mutations::MutationCheckResult first, check_mutations::MutationCheckResult second) noexcept{return check_mutations::MutationCheckResult{ast::diagnostics_append(first.diagnostics, second.diagnostics), second.mutable_locals};}

check_mutations::MutationCheckResult dispatch_mutations_pass(check_mutations::MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> check_mutations::MutationCheckResult { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressionint; return MutationsPass_visit_int(mutations_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [string_value, type_value, _w0] = _v_semanticexpressionstr; return MutationsPass_visit_str(mutations_pass, string_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [boolean_value, type_value, _w0] = _v_semanticexpressionbool; return MutationsPass_visit_bool(mutations_pass, boolean_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionunit; return MutationsPass_visit_unit(mutations_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*expression)._)) { auto _v_semanticexpressionfloat = std::get<semantic_ir::SemanticExpressionFloat>((*expression)._); auto [float_value, type_value, _w0] = _v_semanticexpressionfloat; return MutationsPass_visit_float(mutations_pass, float_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*expression)._)) { auto _v_semanticexpressioni64 = std::get<semantic_ir::SemanticExpressionI64>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressioni64; return MutationsPass_visit_i64(mutations_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*expression)._)) { auto _v_semanticexpressionu8 = std::get<semantic_ir::SemanticExpressionU8>((*expression)._); auto [byte_value, type_value, _w0] = _v_semanticexpressionu8; return MutationsPass_visit_u8(mutations_pass, byte_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*expression)._)) { auto _v_semanticexpressionusize = std::get<semantic_ir::SemanticExpressionUsize>((*expression)._); auto [size_value, type_value, _w0] = _v_semanticexpressionusize; return MutationsPass_visit_usize(mutations_pass, size_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*expression)._)) { auto _v_semanticexpressionchar = std::get<semantic_ir::SemanticExpressionChar>((*expression)._); auto [character_value, type_value, _w0] = _v_semanticexpressionchar; return MutationsPass_visit_char(mutations_pass, character_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*expression)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionextern; return MutationsPass_visit_extern(mutations_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name_value, type_value, _w0] = _v_semanticexpressionident; return MutationsPass_visit_ident(mutations_pass, name_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left_expression, right_expression, type_value, _w0] = _v_semanticexpressionbin; return MutationsPass_visit_bin(mutations_pass, operation, left_expression, right_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*expression)._)) { auto _v_semanticexpressionun = std::get<semantic_ir::SemanticExpressionUn>((*expression)._); auto [operation, inner_expression, type_value, _w0] = _v_semanticexpressionun; return MutationsPass_visit_un(mutations_pass, operation, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function_expression, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressioncall; return MutationsPass_visit_call(mutations_pass, function_expression, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressionmethod; return MutationsPass_visit_method(mutations_pass, object, method_name, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [object, field_name, type_value, _w0] = _v_semanticexpressionfield; return MutationsPass_visit_field(mutations_pass, object, field_name, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [object, index_expression, type_value, _w0] = _v_semanticexpressionindex; return MutationsPass_visit_index(mutations_pass, object, index_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, type_value, _w0] = _v_semanticexpressionif; return MutationsPass_visit_if(mutations_pass, condition, then_expression, else_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, type_value, _w0] = _v_semanticexpressionblock; return MutationsPass_visit_block(mutations_pass, statements, result_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, type_value, _w0] = _v_semanticexpressionwhile; return MutationsPass_visit_while(mutations_pass, condition, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable, iterator, statements, type_value, _w0] = _v_semanticexpressionfor; return MutationsPass_visit_for(mutations_pass, variable, iterator, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, type_value, _w0] = _v_semanticexpressionmatch; return MutationsPass_visit_match(mutations_pass, subject, arms, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*expression)._)) { auto _v_semanticexpressionrecord = std::get<semantic_ir::SemanticExpressionRecord>((*expression)._); auto [type_name, field_values, type_value, _w0] = _v_semanticexpressionrecord; return MutationsPass_visit_record(mutations_pass, type_name, field_values, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._)) { auto _v_semanticexpressionrecordupdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._); auto [type_name, base_expression, overrides, type_value, _w0] = _v_semanticexpressionrecordupdate; return MutationsPass_visit_record_update(mutations_pass, type_name, base_expression, overrides, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressionarray; return MutationsPass_visit_array(mutations_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*expression)._)) { auto _v_semanticexpressiontuple = std::get<semantic_ir::SemanticExpressionTuple>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressiontuple; return MutationsPass_visit_tuple(mutations_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, type_value, _w0] = _v_semanticexpressionquestion; return MutationsPass_visit_question(mutations_pass, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*expression)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*expression)._); auto [parameter_names, body_expression, type_value, _w0] = _v_semanticexpressionlambda; return MutationsPass_visit_lambda(mutations_pass, parameter_names, body_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [resource, binder, statements, type_value, _w0] = _v_semanticexpressionwith; return MutationsPass_visit_with(mutations_pass, resource, binder, statements, type_value); } return MutationsPass_visit_unsupported(mutations_pass, expression); }();}

check_mutations::MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, check_mutations::MutationsPass mutations_pass) noexcept{return field_values.fold(mutation_check_ok(mutations_pass.mutable_locals), [mutations_pass](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticFieldVal> field_value_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, field_value_under_walk->value)); });}

check_mutations::MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, check_mutations::MutationsPass mutations_pass) noexcept{return elements.fold(mutation_check_ok(mutations_pass.mutable_locals), [mutations_pass](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticExpression> element_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, element_under_walk)); });}

check_mutations::MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{return statements.fold(check_mutations::MutationCheckResult{{}, mutable_locals}, [](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticStatement> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> check_mutations::MutationCheckResult { auto [name, is_mut, value, _w0, _w1] = semanticstatementlet; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after = mutable_scope_after_let(accumulator.mutable_locals, name, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), scope_after};
 }(); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> check_mutations::MutationCheckResult { auto [pattern, is_mut, value, _w0, _w1, _w2, _w3] = semanticstatementletpattern; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.mutable_locals;
  mlc::Array<mlc::String> scope_after_bindings = mutable_scope_after_pattern_let(scope_after_value, pattern, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, scope_after_value)), scope_after_bindings};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> check_mutations::MutationCheckResult { auto [name, value, _w0, _w1] = semanticstatementletconst; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> check_mutations::MutationCheckResult { auto [expression, _w0] = semanticstatementexpr; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> check_mutations::MutationCheckResult { auto [expression, _w0] = semanticstatementreturn; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> check_mutations::MutationCheckResult { auto [_w0] = semanticstatementbreak; return accumulator; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> check_mutations::MutationCheckResult { auto [_w0] = semanticstatementcontinue; return accumulator; }
}, (*statement_under_walk)._); });}

check_mutations::MutationCheckResult MutationsPass_visit_int(check_mutations::MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_str(check_mutations::MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_bool(check_mutations::MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_unit(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_float(check_mutations::MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_i64(check_mutations::MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_u8(check_mutations::MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_usize(check_mutations::MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_char(check_mutations::MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_extern(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_ident(check_mutations::MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_bin(check_mutations::MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult right_check = dispatch_mutations_pass(self, right_expression);
return operation == mlc::String("=") ? [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<ast::Diagnostic> assignment_errors = [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*left_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*left_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return !scope_has(self.mutable_locals, name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("cannot assign to immutable binding: ") + name, semantic_ir::sexpr_span(left_expression), diagnostic_codes::diagnostic_code_e031())} : mutation_check_empty(); } return mutation_check_empty(); }();
  return check_mutations::MutationCheckResult{ast::diagnostics_append(assignment_errors, right_check.diagnostics), self.mutable_locals};
 }() : merge_mutation_check_results(dispatch_mutations_pass(self, left_expression), right_check);
}

check_mutations::MutationCheckResult MutationsPass_visit_un(check_mutations::MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, inner_expression);}

check_mutations::MutationCheckResult MutationsPass_visit_call(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, function_expression), check_mutation_semantic_elements(arguments, self));}

check_mutations::MutationCheckResult MutationsPass_visit_method(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, object), check_mutation_semantic_elements(arguments, self));}

check_mutations::MutationCheckResult MutationsPass_visit_field(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, object);}

check_mutations::MutationCheckResult MutationsPass_visit_index(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, object), dispatch_mutations_pass(self, index_expression));}

check_mutations::MutationCheckResult MutationsPass_visit_if(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, condition), merge_mutation_check_results(dispatch_mutations_pass(self, then_expression), dispatch_mutations_pass(self, else_expression)));}

check_mutations::MutationCheckResult MutationsPass_visit_block(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
check_mutations::MutationCheckResult result_check = dispatch_mutations_pass(mutations_pass_new(statements_check.mutable_locals), result_expression);
return check_mutations::MutationCheckResult{ast::diagnostics_append(statements_check.diagnostics, result_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_while(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, condition).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_for(check_mutations::MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, iterator).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_match(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult accumulator = dispatch_mutations_pass(self, subject);
int arm_index = 0;
while (arm_index < arms.size()){
{
std::shared_ptr<semantic_ir::SemanticMatchArm> arm_under_walk = arms[arm_index];
mlc::Array<mlc::String> arm_scope = mutable_scope_with_bindings(self.mutable_locals, names::pattern_bindings(arm_under_walk->pattern));
check_mutations::MutationsPass arm_pass = mutations_pass_new(arm_scope);
if (arm_under_walk->has_guard){
{
accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->when_condition));
}
}
accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->body));
arm_index = arm_index + 1;
}
}
return check_mutations::MutationCheckResult{accumulator.diagnostics, self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_record(check_mutations::MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_field_values(field_values, self);}

check_mutations::MutationCheckResult MutationsPass_visit_record_update(check_mutations::MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, base_expression), check_mutation_semantic_field_values(overrides, self));}

check_mutations::MutationCheckResult MutationsPass_visit_array(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_elements(elements, self);}

check_mutations::MutationCheckResult MutationsPass_visit_tuple(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_elements(elements, self);}

check_mutations::MutationCheckResult MutationsPass_visit_question(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, inner_expression);}

check_mutations::MutationCheckResult MutationsPass_visit_lambda(check_mutations::MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
mlc::Array<mlc::String> empty_scope = {};
return dispatch_mutations_pass(mutations_pass_new(empty_scope), body_expression);
}

check_mutations::MutationCheckResult MutationsPass_visit_with(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, resource).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_unsupported(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{return mutation_check_ok(self.mutable_locals);}

mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept{
check_mutations::MutationCheckResult check_result = dispatch_mutations_pass(mutations_pass_new(mutable_locals), expression);
return check_result.diagnostics;
}

mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
mlc::Array<mlc::String> mutable_locals = parameters.fold([&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }(), [](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return mutable_scope_after_let(parameter_scope, parameter_under_check->name, parameter_under_check->is_mut); });
return check_mutation_semantic_expression(body, mutable_locals);
}

} // namespace check_mutations
