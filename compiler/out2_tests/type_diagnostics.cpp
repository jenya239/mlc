#define main mlc_user_main
#include "type_diagnostics.hpp"

namespace type_diagnostics {

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
if ((((operation == mlc::String("-", 1)) && (!semantic_type_structure::type_is_unknown(inner_type))) && (!semantic_type_structure::type_is_i32(inner_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("unary - expects i32, got ", 25) + semantic_type_structure::type_description(inner_type)), source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
if ((((operation == mlc::String("!", 1)) && (!semantic_type_structure::type_is_unknown(inner_type))) && (!semantic_type_structure::type_is_bool(inner_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("operator ! expects bool, got ", 29) + semantic_type_structure::type_description(inner_type)), source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept{
if ((expected_length != actual_length)) {
return mlc::Array{ast::diagnostic_error((((mlc::String("expected ", 9) + mlc::to_string(expected_length)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_length)), call_source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
auto expected = (array_method_types::is_array_hof_method(method_name) ? array_method_types::array_hof_expected_argument_count(method_name) : semantic_type_structure::builtin_method_expected_argument_count(method_name));
if ((expected < 0)) {
return {};
} else if ((argument_count != expected)) {
return mlc::Array{ast::diagnostic_error((((mlc::String("expected ", 9) + mlc::to_string(expected)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(argument_count)), method_span)};
} else {
return {};
}

}
mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept{
if ((expected_count != actual_count)) {
return mlc::Array{ast::diagnostic_error((((mlc::String("expected ", 9) + mlc::to_string(expected_count)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_count)), call_source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> call_arity_range_diagnostics(int min_count, int max_count, int actual_count, ast::Span call_source_span) noexcept{
if (((actual_count < min_count) || (actual_count > max_count))) {
return mlc::Array{ast::diagnostic_error((((((mlc::String("expected between ", 17) + mlc::to_string(min_count)) + mlc::String(" and ", 5)) + mlc::to_string(max_count)) + mlc::String(" arguments, got ", 16)) + mlc::to_string(actual_count)), call_source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
if ((receiver_errors.length() > 0)) {
return {};
} else {
return infer_builtin_method_arity_diagnostics(method_name, argument_count, method_span);
}
}
mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept{
if (((!semantic_type_structure::type_is_unknown(receiver_type)) && (!semantic_type_structure::type_is_array(receiver_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("indexing requires an array, got ", 32) + semantic_type_structure::type_description(receiver_type)), bracket_source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept{
if (((!semantic_type_structure::type_is_unknown(index_type)) && (!semantic_type_structure::type_is_i32(index_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("array index must be i32, got ", 29) + semantic_type_structure::type_description(index_type)), bracket_source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept{
if (branches_mismatch) {
return mlc::Array{ast::diagnostic_error((((mlc::String("if branch types differ: then ", 29) + semantic_type_structure::type_description(then_type)) + mlc::String(", else ", 7)) + semantic_type_structure::type_description(else_type)), ast::expr_span(else_expression))};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept{
if (((!semantic_type_structure::type_is_unknown(iterator_type)) && (!semantic_type_structure::type_is_array(iterator_type)))) {
return mlc::Array{ast::diagnostic_error((mlc::String("for loop range must be an array, got ", 37) + semantic_type_structure::type_description(iterator_type)), ast::expr_span(iterator))};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept{
if (((((arm_index > 0) && (!semantic_type_structure::type_is_unknown(first_arm_type))) && (!semantic_type_structure::type_is_unknown(arm_result_type))) && (!semantic_type_structure::types_structurally_equal(first_arm_type, arm_result_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("match arm result type ", 22) + semantic_type_structure::type_description(arm_result_type)) + mlc::String(" does not match first arm type ", 31)) + semantic_type_structure::type_description(first_arm_type)), ast::expr_span(arm_body))};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept{
auto type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*object_type));
if (((type_name != mlc::String("", 0)) && registry::TypeRegistry_has_fields(registry, type_name))) {
if ((!registry::TypeRegistry_fields_for(registry, type_name).has(field_name))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("undefined field: ", 17) + field_name) + mlc::String(" on ", 4)) + type_name), field_source_span)};
} else {
return {};
}
} else if ((!semantic_type_structure::type_is_unknown(object_type))) {
if (((type_name != mlc::String("", 0)) && (!registry::TypeRegistry_has_fields(registry, type_name)))) {
return mlc::Array{ast::diagnostic_error(((mlc::String("field access needs a record type; ", 34) + type_name) + mlc::String(" is not one", 11)), field_source_span)};
} else if (semantic_type_structure::type_is_array(object_type)) {
return mlc::Array{ast::diagnostic_error((mlc::String("array value has no record fields for .", 38) + field_name), field_source_span)};
} else if ((!semantic_type_structure::type_is_function(object_type))) {
return mlc::Array{ast::diagnostic_error((mlc::String("field access on value without record fields: ", 45) + semantic_type_structure::type_description(object_type)), field_source_span)};
} else {
return {};
}


} else {
return {};
}

}

} // namespace type_diagnostics
