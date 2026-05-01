#define main mlc_user_main
#include "binary_diagnostics.hpp"

namespace binary_diagnostics {

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((((operation == mlc::String("+", 1)) && (!semantic_type_structure::type_is_unknown(left_type))) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_binary_plus(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("incompatible operand types for +: ", 34) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((((operation == mlc::String("-", 1)) || (operation == mlc::String("*", 1))) || (operation == mlc::String("/", 1))) || (operation == mlc::String("%", 1)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((((mlc::String("numeric operands required for ", 30) + operation) + mlc::String(", got ", 6)) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((operation == mlc::String("==", 2)) || (operation == mlc::String("!=", 2)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_structurally_equal(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((((mlc::String("incompatible types for ", 23) + operation) + mlc::String(": ", 2)) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((((operation == mlc::String("<", 1)) || (operation == mlc::String(">", 1))) || (operation == mlc::String("<=", 2))) || (operation == mlc::String(">=", 2)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_ordering(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("ordering requires i32 or string operands of the same kind, got ", 63) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((operation == mlc::String("&&", 2)) || (operation == mlc::String("||", 2)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_logical_and_or(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("logical operators require bool operands, got ", 45) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if ((((operation == mlc::String("&", 1)) || (operation == mlc::String("|", 1))) || (operation == mlc::String("^", 1)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((((mlc::String("bitwise ", 8) + operation) + mlc::String(" requires i32 operands, got ", 28)) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
if (((operation == mlc::String("<<", 2)) || (operation == mlc::String(">>", 2)))) {
if ((((!semantic_type_structure::type_is_unknown(left_type)) && (!semantic_type_structure::type_is_unknown(right_type))) && (!semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type)))) {
return mlc::Array{ast::diagnostic_error((((mlc::String("shift requires i32 operands, got ", 33) + semantic_type_structure::type_description(left_type)) + mlc::String(" and ", 5)) + semantic_type_structure::type_description(right_type)), source_span)};
} else {
return {};
}
} else {
return {};
}
}
mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
auto from_addition = infer_binary_diagnostic_for_addition(operation, left_type, right_type, source_span);
if ((from_addition.length() > 0)) {
return from_addition;
} else {
auto from_int_arithmetic = infer_binary_diagnostic_for_int_arithmetic(operation, left_type, right_type, source_span);
if ((from_int_arithmetic.length() > 0)) {
return from_int_arithmetic;
} else {
auto from_equality = infer_binary_diagnostic_for_equality(operation, left_type, right_type, source_span);
if ((from_equality.length() > 0)) {
return from_equality;
} else {
auto from_ordering = infer_binary_diagnostic_for_ordering(operation, left_type, right_type, source_span);
if ((from_ordering.length() > 0)) {
return from_ordering;
} else {
auto from_logical = infer_binary_diagnostic_for_logical(operation, left_type, right_type, source_span);
if ((from_logical.length() > 0)) {
return from_logical;
} else {
auto from_bitwise = infer_binary_diagnostic_for_bitwise(operation, left_type, right_type, source_span);
if ((from_bitwise.length() > 0)) {
return from_bitwise;
} else {
return infer_binary_diagnostic_for_shift(operation, left_type, right_type, source_span);
}
}
}
}
}
}
}

} // namespace binary_diagnostics
