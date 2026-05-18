#define main mlc_user_main
#include "infer.hpp"

namespace infer {

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept{
return expressions.fold(initial, [=](infer_result::InferResult accumulated_inference, std::shared_ptr<ast::Expr> expression_under_inference) mutable { return InferResult_absorb(accumulated_inference, infer_expr(expression_under_inference, inference_context)); });
}
infer_result::InferResult infer_record_field_binding_value_inference_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
auto registry_has_ordered_fields_for_type = ((record_type_name_for_fields.length() > 0) && registry::TypeRegistry_has_fields(inference_context.registry, record_type_name_for_fields));
auto next_result = [&]() -> infer_result::InferResult {
if (registry_has_ordered_fields_for_type) {
return infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, ast::expr_span(field_value->val), inference_context.registry))};
} else {
return accumulator;
}
}();
auto value_inference = infer_expr(field_value->val, inference_context);
if (registry_has_ordered_fields_for_type) {
auto expected_field_type = registry::field_type_from_object(std::make_shared<registry::Type>(registry::TNamed{record_type_name_for_fields}), field_value->name, inference_context.registry);
auto inferred_value_type = value_inference.inferred_type;
auto field_types_conflict = (((!semantic_type_structure::type_is_unknown(expected_field_type)) && (!semantic_type_structure::type_is_unknown(inferred_value_type))) && (!semantic_type_structure::types_structurally_equal(expected_field_type, inferred_value_type)));
auto field_type_mismatch_diagnostics = (field_types_conflict ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error((((((mlc::String("field ", 6) + field_value->name) + mlc::String(": expected ", 11)) + semantic_type_structure::type_description(expected_field_type)) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(inferred_value_type)), ast::expr_span(field_value->val))}) : (mlc::Array<ast::Diagnostic>{}));
return infer_result::InferResult{next_result.inferred_type, ast::diagnostics_append(ast::diagnostics_append(next_result.errors, value_inference.errors), field_type_mismatch_diagnostics)};
} else {
return InferResult_absorb(next_result, value_inference);
}
}
infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept{
return field_values.fold(initial, [=](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_under_inference) mutable { return infer_record_field_binding_value_inference_step(accumulator, field_value_under_inference, inference_context, record_type_name_for_fields); });
}
infer_result::InferResult infer_explicit_record_literal_field_unknown_name_step(infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding, mlc::String record_type_name, registry::TypeRegistry registry) noexcept{
return infer_result::InferResult{accumulator.inferred_type, ast::diagnostics_append(accumulator.errors, type_diagnostics::infer_expr_field_diagnostics(std::make_shared<registry::Type>(registry::TNamed{record_type_name}), field_value_binding->name, ast::expr_span(field_value_binding->val), registry))};
}
infer_result::InferResult infer_explicit_record_literal_field_name_errors(infer_result::InferResult initial, mlc::String record_type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> explicit_field_values_flat, registry::TypeRegistry registry) noexcept{
return explicit_field_values_flat.fold(initial, [=](infer_result::InferResult accumulator, std::shared_ptr<ast::FieldVal> field_value_binding) mutable { return infer_explicit_record_literal_field_unknown_name_step(accumulator, field_value_binding, record_type_name, registry); });
}
infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept{
auto left_result = infer_expr(left, inference_context);
auto right_result = infer_expr(right, inference_context);
auto method = semantic_type_structure::operator_method_for(operation);
if ((method != mlc::String("", 0))) {
auto method_type = registry::method_return_type_from_object(left_result.inferred_type, method, inference_context.registry);
if ((!semantic_type_structure::type_is_unknown(method_type))) {
return infer_result::InferResult{method_type, ast::diagnostics_append(left_result.errors, right_result.errors)};
} else {
return infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
}
} else {
return infer_operand_combine::infer_binary_from_operand_results(operation, left_result, right_result, source_span);
}
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
infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, check_context::CheckContext inference_context) noexcept{
auto res = infer_expr(resource, inference_context);
auto inner_env = inference_context.type_env;
inner_env.set(binder, res.inferred_type);
auto stmts_result = infer_statements(stmts, check_context::check_context_new(inner_env, inference_context.registry));
return InferResult_with_type(InferResult_absorb_stmt(res, stmts_result), std::make_shared<registry::Type>(registry::TUnit{}));
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
infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept{
auto base_result = infer_expr(base, inference_context);
return infer_field_values_errors(InferResult_with_type(base_result, std::make_shared<registry::Type>(registry::TNamed{type_name})), field_values, inference_context, type_name);
}
Infer_tuple_literal_fold_state infer_tuple_literal_element_fold_step(Infer_tuple_literal_fold_state state, std::shared_ptr<ast::Expr> tuple_element, check_context::CheckContext inference_context) noexcept{
auto element_inference = infer_expr(tuple_element, inference_context);
state.member_types.push_back(element_inference.inferred_type);
return Infer_tuple_literal_fold_state{InferResult_absorb(state.combined_inference, element_inference), state.member_types};
}
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
if ((elements.length() < 2)) {
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{})), elements, inference_context);
} else {
auto folded_tuple_inference = elements.fold(Infer_tuple_literal_fold_state{infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{})), {}}, [=](Infer_tuple_literal_fold_state accumulator_state, std::shared_ptr<ast::Expr> tuple_element_under_inference) mutable { return infer_tuple_literal_element_fold_step(accumulator_state, tuple_element_under_inference, inference_context); });
return InferResult_with_type(folded_tuple_inference.combined_inference, std::make_shared<registry::Type>(registry::TTuple{folded_tuple_inference.member_types}));
}
}
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept{
auto first_element_type = ((elements.length() > 0) ? (infer_expr(elements[0], inference_context).inferred_type) : (std::make_shared<registry::Type>(registry::TUnknown{})));
return infer_arguments_errors(infer_result::infer_ok(std::make_shared<registry::Type>(registry::TArray{first_element_type})), elements, inference_context);
}
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept{
return infer_question_expression::infer_question_from_inner_result(infer_expr(inner, inference_context), question_span);
}
std::shared_ptr<registry::Type> inference_placeholder_unknown_type() noexcept{
return std::make_shared<registry::Type>(registry::TUnknown{});
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_inference_environment_assign_unknown_placeholder(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment_for_parameters, mlc::String parameter_binding_name) noexcept{
type_environment_for_parameters.set(parameter_binding_name, inference_placeholder_unknown_type());
return type_environment_for_parameters;
}
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept{
auto lambda_type_environment = inference_context.type_env;
auto inferred_unknown_parameter_types = parameter_names.map([=](mlc::String unused_lambda_parameter_placeholder) mutable { return inference_placeholder_unknown_type(); });
parameter_names.fold(lambda_type_environment, [=](mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> environment_map, mlc::String parameter_binding_name) mutable { return lambda_inference_environment_assign_unknown_placeholder(environment_map, parameter_binding_name); });
auto lambda_inference_context = check_context::check_context_new(lambda_type_environment, inference_context.registry);
auto body_inference = infer_expr(body, lambda_inference_context);
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TFn{inferred_unknown_parameter_types, body_inference.inferred_type}));
}
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept{
auto expression_partial_application = partial_application_desugar::partial_application_desugar_expr(expression);
return std::visit(overloaded{[&](const ast::ExprInt& exprInt) { auto [__0, __1] = exprInt; return infer_literals::infer_expr_integer_literal(); },
[&](const ast::ExprStr& exprStr) { auto [__0, __1] = exprStr; return infer_literals::infer_expr_string_literal(); },
[&](const ast::ExprBool& exprBool) { auto [__0, __1] = exprBool; return infer_literals::infer_expr_boolean_literal(); },
[&](const ast::ExprUnit& exprUnit) { auto [__0] = exprUnit; return infer_literals::infer_expr_unit_literal(); },
[&](const ast::ExprFloat& exprFloat) { auto [__0, __1] = exprFloat; return infer_literals::infer_expr_f64_literal(); },
[&](const ast::ExprI64& exprI64) { auto [__0, __1] = exprI64; return infer_literals::infer_expr_i64_literal(); },
[&](const ast::ExprU8& exprU8) { auto [__0, __1] = exprU8; return infer_literals::infer_expr_u8_literal(); },
[&](const ast::ExprUsize& exprUsize) { auto [__0, __1] = exprUsize; return infer_literals::infer_expr_usize_literal(); },
[&](const ast::ExprChar& exprChar) { auto [__0, __1] = exprChar; return infer_literals::infer_expr_char_literal(); },
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
[&](const ast::ExprRecord& exprRecord) { auto [name, lit_parts, span] = exprRecord; return infer_expr_record(name, lit_parts, inference_context, span); },
[&](const ast::ExprRecordUpdate& exprRecordUpdate) { auto [name, base, field_vals, __3] = exprRecordUpdate; return infer_expr_record_update(name, base, field_vals, inference_context); },
[&](const ast::ExprArray& exprArray) { auto [elements, __1] = exprArray; return infer_expr_array_literal(elements, inference_context); },
[&](const ast::ExprTuple& exprTuple) { auto [elements, __1] = exprTuple; return infer_expr_tuple_literal(elements, inference_context); },
[&](const ast::ExprQuestion& exprQuestion) { auto [inner, question_span] = exprQuestion; return infer_expr_question(inner, question_span, inference_context); },
[&](const ast::ExprLambda& exprLambda) { auto [params, body, __2] = exprLambda; return infer_expr_lambda(params, body, inference_context); },
[&](const ast::ExprNamedArg& exprNamedArg) { auto [__0, inner, __2] = exprNamedArg; return infer_expr(inner, inference_context); },
[&](const ast::ExprWith& exprWith) { auto [resource, binder, stmts, __3] = exprWith; return infer_expr_with(resource, binder, stmts, inference_context); }
}, (*expression_partial_application));
}
Record_literal_spread_inference_fold_state accumulate_record_literal_spread_inference_for_literal_part(Record_literal_spread_inference_fold_state fold_state, ast::RecordLitPart literal_part_under_inference, check_context::CheckContext inference_context) noexcept{
return std::visit(overloaded{[&](const ast::RecordLitSpread& recordLitSpread) { auto [spread_expression_under_inference] = recordLitSpread; return [&]() {
auto spread_expression_inference = infer_expr(spread_expression_under_inference, inference_context);
fold_state.spread_inferred_types_in_visit_order.push_back(spread_expression_inference.inferred_type);
return Record_literal_spread_inference_fold_state{InferResult_absorb(fold_state.inference_result_so_far, spread_expression_inference), fold_state.spread_inferred_types_in_visit_order};
}(); },
[&](const ast::RecordLitFields& recordLitFields) { auto [__0] = recordLitFields; return fold_state; }
}, literal_part_under_inference);
}
Record_literal_spread_inference_fold_state infer_record_literal_fold_spread_inference_parts(infer_result::InferResult inference_before_spread_merge, mlc::Array<ast::RecordLitPart> literal_parts_under_inference, check_context::CheckContext inference_context) noexcept{
return literal_parts_under_inference.fold(Record_literal_spread_inference_fold_state{inference_before_spread_merge, {}}, [=](Record_literal_spread_inference_fold_state accumulator_fold_state, ast::RecordLitPart literal_part_under_inference) mutable { return accumulate_record_literal_spread_inference_for_literal_part(accumulator_fold_state, literal_part_under_inference, inference_context); });
}
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, check_context::CheckContext inference_context, ast::Span span) noexcept{
auto private_errors = ((registry::TypeRegistry_is_private_ctor(inference_context.registry, type_name) && (inference_context.current_extend_type != type_name)) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error(((mlc::String("private constructor: cannot construct ", 38) + type_name) + mlc::String(" outside its extend block", 25)), span)}) : (mlc::Array<ast::Diagnostic>{}));
auto result = infer_result::InferResult{std::make_shared<registry::Type>(registry::TNamed{type_name}), private_errors};
auto explicit_field_values_flat = record_lit_merge::collect_explicit_field_values_flat(lit_parts);
if ((!record_lit_merge::record_literal_contains_spread(lit_parts))) {
if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)) {
result = infer_explicit_record_literal_field_name_errors(result, type_name, explicit_field_values_flat, inference_context.registry);
}
auto merged_with_optional_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, explicit_field_values_flat, inference_context.registry);
result = infer_field_values_errors(result, merged_with_optional_defaults, inference_context, type_name);
if (registry::TypeRegistry_has_fields(inference_context.registry, type_name)) {
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_with_optional_defaults, span))};
}
return result;
} else if ((!registry::TypeRegistry_has_fields(inference_context.registry, type_name))) {
result = infer_field_values_errors(result, explicit_field_values_flat, inference_context, mlc::String("", 0));
return infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("record literal spread requires a named record type with fields", 62), span)})};
} else {
result = infer_field_values_errors(result, explicit_field_values_flat, inference_context, type_name);
auto spread_inference_after_literal_parts = infer_record_literal_fold_spread_inference_parts(result, lit_parts, inference_context);
result = spread_inference_after_literal_parts.inference_result_so_far;
auto spread_types_in_order = spread_inference_after_literal_parts.spread_inferred_types_in_visit_order;
auto merged_field_values = record_lit_merge::merge_record_literal_parts_to_field_values(type_name, lit_parts, inference_context.registry, spread_types_in_order);
auto merged_including_decl_defaults = record_lit_merge::merge_explicit_record_literal_with_type_defaults(type_name, merged_field_values, inference_context.registry);
result = infer_field_values_errors(result, merged_including_decl_defaults, inference_context, type_name);
result = infer_result::InferResult{result.inferred_type, ast::diagnostics_append(result.errors, record_lit_merge::diagnostics_missing_fields_for_ordered_record(type_name, inference_context.registry, merged_including_decl_defaults, span))};
return result;
}

}
Statement_sequence_inference_fold_state infer_single_statement_augment_fold_state(Statement_sequence_inference_fold_state accumulator, std::shared_ptr<ast::Stmt> statement_under_inference, registry::TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ast::StmtLet& stmtLet) { auto [binding_name, __1, __2, value_expression, __4] = stmtLet; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtLetPat& stmtLetPat) { auto [pattern, __1, __2, value_expression, has_else, else_body, source_span] = stmtLetPat; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
let_pattern_infer::infer_let_pattern_env(pattern, value_inference.inferred_type, accumulator.type_environment_under_construction, registry);
auto next_diagnostics = ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors);
auto pattern_is_refutable = std::visit(overloaded{[&](const ast::PatCtor& patCtor) { auto [__0, __1, __2] = patCtor; return true; },
[&](const auto& __v) { return false; }
}, (*pattern));
if ((pattern_is_refutable && (!has_else))) {
next_diagnostics = ast::diagnostics_append(next_diagnostics, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("refutable pattern in let requires `else` branch", 47), source_span)});
}
if (has_else) {
auto else_branch_inference = infer_expr(else_body, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
next_diagnostics = ast::diagnostics_append(next_diagnostics, else_branch_inference.errors);
}
return Statement_sequence_inference_fold_state{next_diagnostics, accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtLetConst& stmtLetConst) { auto [binding_name, __1, value_expression, __3] = stmtLetConst; return [&]() {
auto value_inference = infer_expr(value_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
accumulator.type_environment_under_construction.set(binding_name, value_inference.inferred_type);
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, value_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtExpr& stmtExpr) { auto [expression, __1] = stmtExpr; return [&]() {
auto expression_inference = infer_expr(expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, expression_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtReturn& stmtReturn) { auto [return_expression, __1] = stmtReturn; return [&]() {
auto return_inference = infer_expr(return_expression, check_context::check_context_new(accumulator.type_environment_under_construction, registry));
return Statement_sequence_inference_fold_state{ast::diagnostics_append(accumulator.diagnostics_collected_so_far, return_inference.errors), accumulator.type_environment_under_construction};
}(); },
[&](const ast::StmtBreak& stmtBreak) { auto [__0] = stmtBreak; return accumulator; },
[&](const ast::StmtContinue& stmtContinue) { auto [__0] = stmtContinue; return accumulator; }
}, (*statement_under_inference));
}
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept{
auto inference_after_statement_sequence = statements.fold(Statement_sequence_inference_fold_state{{}, inference_context.type_env}, [=](Statement_sequence_inference_fold_state accumulator_fold_state, std::shared_ptr<ast::Stmt> statement_under_inference_within_block) mutable { return infer_single_statement_augment_fold_state(accumulator_fold_state, statement_under_inference_within_block, inference_context.registry); });
return infer_result::StmtInferResult{inference_after_statement_sequence.type_environment_under_construction, inference_after_statement_sequence.diagnostics_collected_so_far};
}

} // namespace infer
