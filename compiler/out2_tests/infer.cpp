#define main mlc_user_main
#include "infer.hpp"

namespace infer {

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{
auto result = initial;
auto index = 0;
while ((index < expressions.length())) {
result = InferResult_absorb(result, infer_expr(expressions[index], inference_context));
index = (index + 1);
}
return result;
}
infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
auto result = initial;
auto index = 0;
while ((index < field_values.length())) {
auto field_value = field_values[index];
auto has_fields = ((record_type_name_for_fields.length() > 0) && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields));
if (has_fields) {
auto field_name_errors = type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, ast::expr_span(field_value->val), inference_context.registry);
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, field_name_errors)};
}
auto value_result = infer_expr(field_value->val, inference_context);
if (has_fields) {
auto expected_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, inference_context.registry);
auto actual_type = value_result.inferred_type;
auto type_mismatch = (((!semantic_type_structure::type_is_unknown(expected_type)) && (!semantic_type_structure::type_is_unknown(actual_type))) && (!semantic_type_structure::types_structurally_equal(expected_type, actual_type)));
auto type_errors = (type_mismatch ? mlc::Array{ast::diagnostic_error((((((mlc::String("field ", 6) + field_value->name) + mlc::String(": expected ", 11)) + semantic_type_structure::type_description(expected_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(actual_type)), ast::expr_span(field_value->val))} : {});
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(ast::diagnostics_append(result.errors, value_result.errors), type_errors)};
} else {
result = InferResult_absorb(result, value_result);
}
index = (index + 1);
}
return result;
}
infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
auto left_result = infer_expr(left, inference_context);
auto right_result = infer_expr(right, inference_context);
return infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
}
infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
return infer_operand_combine::infer_unary_from_inner_result(operation, infer_expr(inner, inference_context), source_span);
}
infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
auto object_result = infer_expr_fn(object, inference_context);
if (infer_result_option_method::should_infer_as_result_option_hof(object_result.inferred_type, method_name)) {
return infer_result_option_method::infer_result_option_hof_method_call(object_result, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
} else if (infer_array_method::should_infer_as_array_hof(object_result.inferred_type, method_name)) {
return infer_array_method::infer_array_hof_method_call(object_result, method_name, method_arguments, method_span, inference_context, infer_expr_fn);
} else {
auto with_arguments = infer_arguments_errors(object_result, method_arguments, inference_context);
return infer_operand_combine::infer_method_from_object_and_arguments(object_result, with_arguments, method_name, method_span, method_arguments.length(), inference_context.registry);
}

}
infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept{
return infer_operand_combine::infer_field_from_object_result(infer_expr(object, inference_context), field_name, field_source_span, inference_context.registry);
}
infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept{
return infer_operand_combine::infer_index_from_operand_results(infer_expr(object, inference_context), infer_expr(index_expression, inference_context), bracket_source_span);
}
infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept{
return infer_operand_combine::infer_conditional_from_branch_results(infer_expr(condition, inference_context), infer_expr(then_expression, inference_context), infer_expr(else_expression, inference_context), else_expression);
}
infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept{
auto statements_result = infer_statements(statements, inference_context);
auto result_inference = infer_expr(result_expression, check_context::check_context_new(statements_result.type_env, inference_context.registry));
return InferResult_absorb_stmt(result_inference, statements_result);
}
infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
auto condition_result = infer_expr(condition, inference_context);
auto statements_result = infer_statements(statements, inference_context);
return InferResult_with_type(InferResult_absorb_stmt(condition_result, statements_result), std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
auto iterator_base = infer_expr(iterator, inference_context);
auto iterator_result = infer_for_support::infer_for_iterator_with_range_rules(iterator_base, iterator);
auto element_type = infer_for_support::element_type_for_for_iterator(iterator_result.inferred_type);
auto inner_environment = inference_context.type_env;
inner_environment.set(variable_name, element_type);
auto loop_context = check_context::check_context_new(inner_environment, inference_context.registry);
auto statements_result = infer_statements(statements, loop_context);
return InferResult_with_type(InferResult_absorb_stmt(iterator_result, statements_result), std::make_shared<registry::Type>(registry::TUnit{}));
}
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
return infer_field_values_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TNamed{type_name})), field_values, inference_context, type_name);
}
infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
auto base_result = infer_expr(base, inference_context);
return infer_field_values_errors(InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed{type_name})), field_values, inference_context, type_name);
}
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
if ((elements.length() < 2)) {
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{})), elements, inference_context);
} else {
auto r = infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{}));
auto types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto i = 0;
while ((i < elements.length())) {
auto er = infer_expr(elements[i], inference_context);
r = InferResult_absorb(r, er);
types.push_back(er.inferred_type);
i = (i + 1);
}
return InferResult_with_type(r, std::make_shared<registry::Type>(registry::TTuple{types}));
}
}
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
auto first_element_type = ((elements.length() > 0) ? infer_expr(elements[0], inference_context).inferred_type : std::make_shared<registry::Type>(registry::TUnknown{}));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray{first_element_type})), elements, inference_context);
}
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept{
return infer_question_expression::infer_question_from_inner_result(infer_expr(inner, inference_context), question_span);
}
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
auto lambda_environment = inference_context.type_env;
auto parameter_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto parameter_index = 0;
while ((parameter_index < parameter_names.length())) {
parameter_types.push_back(std::make_shared<registry::Type>(registry::TUnknown{}));
lambda_environment.set(parameter_names[parameter_index], std::make_shared<registry::Type>(registry::TUnknown{}));
parameter_index = (parameter_index + 1);
}
auto lambda_context = check_context::check_context_new(lambda_environment, inference_context.registry);
auto body_result = infer_expr(body, lambda_context);
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn{parameter_types, body_result.inferred_type}));
}
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return infer_literals::infer_expr_integer_literal(); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return infer_literals::infer_expr_string_literal(); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return infer_literals::infer_expr_boolean_literal(); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return infer_literals::infer_expr_unit_literal(); },
[&](const ast::ExprExtern& exprExtern) { auto [__0] = exprExtern; return infer_literals::infer_expr_extern_placeholder(); },
[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return infer_expr_ident::infer_expr_identifier(name, inference_context); },
[&](const ast::ExprBin& exprBin) { auto [operation, left, right, span] = exprBin; return infer_expr_binary(operation, left, right, span, inference_context); },
[&](const ast::ExprUn& exprUn) { auto [operation, inner, span] = exprUn; return infer_expr_unary(operation, inner, span, inference_context); },
[&](const ast::ExprCall& exprCall) { auto [function, call_arguments, call_source_span] = exprCall; return infer_call::infer_expr_call(function, call_arguments, call_source_span, inference_context, infer_expr); },
[&](const ast::ExprMethod& exprMethod) { auto [object, method_name, margs, method_span] = exprMethod; return infer_expr_method(object, method_name, margs, method_span, inference_context, infer_expr); },
[&](const ast::ExprField& exprField) { auto [object, field_name, field_source_span] = exprField; return infer_expr_field(object, field_name, field_source_span, inference_context); },
[&](const ast::ExprIndex& exprIndex) { auto [object, index_expression, bracket_source_span] = exprIndex; return infer_expr_index(object, index_expression, bracket_source_span, inference_context); },
[&](const ast::ExprIf& exprIf) { auto [condition, then_expr, else_expr, __3] = exprIf; return infer_expr_conditional(condition, then_expr, else_expr, inference_context); },
[&](const ast::ExprBlock& exprBlock) { auto [statements, result, __2] = exprBlock; return infer_expr_block(statements, result, inference_context); },
[&](const ast::ExprWhile& exprWhile) { auto [condition, statements, __2] = exprWhile; return infer_expr_while_loop(condition, statements, inference_context); },
[&](const ast::ExprFor& exprFor) { auto [variable, iterator, statements, __3] = exprFor; return infer_expr_for_loop(variable, iterator, statements, inference_context); },
[&](const ast::ExprMatch& exprMatch) { auto [subject, arms, __2] = exprMatch; return infer_match::infer_expr_match(subject, arms, inference_context, infer_expr); },
[&](const ast::ExprRecord& exprRecord) { auto [name, field_vals, __2] = exprRecord; return infer_expr_record(name, field_vals, inference_context); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [name, base, field_vals, __3] = exprRecordUpdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return infer_expr_array_literal(elements, inference_context); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return infer_expr_tuple_literal(elements, inference_context); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, question_span] = exprQuestion; return infer_expr_question(inner, question_span, inference_context); },
[&](const ast::ExprLambda& exprLambda) { auto [params, body, __2] = exprLambda; return infer_expr_lambda(params, body, inference_context); }
}, (*expression));
}
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
auto collected_errors = mlc::Array<ast::Diagnostic>{};
auto current_environment = inference_context.type_env;
auto statement_index = 0;
while ((statement_index < statements.length())) {
std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [binding_name, __1, __2, value_expression, __4] = stmtLet; return [&]() {
auto value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
current_environment.set(binding_name, value_result.inferred_type);
collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, __1, __2, value_expression, __4] = stmtLetPat; return [&]() {
auto value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
let_pattern_infer::infer_let_pattern_env(pattern, value_result.inferred_type, current_environment, inference_context.registry);
collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [binding_name, __1, value_expression, __3] = stmtLetConst; return [&]() {
auto value_result = infer_expr(value_expression, check_context::check_context_new(current_environment, inference_context.registry));
current_environment.set(binding_name, value_result.inferred_type);
collected_errors = ast::diagnostics_append(collected_errors, value_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return [&]() {
auto expression_result = infer_expr(expression, check_context::check_context_new(current_environment, inference_context.registry));
collected_errors = ast::diagnostics_append(collected_errors, expression_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [return_expression, __1] = stmtReturn; return [&]() {
auto return_result = infer_expr(return_expression, check_context::check_context_new(current_environment, inference_context.registry));
collected_errors = ast::diagnostics_append(collected_errors, return_result.errors);
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return [&]() {
/* unit */;
return /* unit */;
}(); }
}, (*statements[statement_index]));
statement_index = (statement_index + 1);
}
return infer_result::StmtInferResult{current_environment, collected_errors};
}

} // namespace infer
