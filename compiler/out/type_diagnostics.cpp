#include "type_diagnostics.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace type_diagnostics {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept;

mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept;

mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept;

mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_addition(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{return operation == mlc::String("+") && !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_plus(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("incompatible operand types for +: ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{};}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_int_arithmetic(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("numeric operands required for ") + operation + mlc::String(", got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_equality(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("==") || operation == mlc::String("!=") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_structurally_equal(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("incompatible types for ") + operation + mlc::String(": ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_ordering(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("<") || operation == mlc::String(">") || operation == mlc::String("<=") || operation == mlc::String(">=") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_ordering(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("ordering requires i32 or string operands of the same kind, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_logical(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("&&") || operation == mlc::String("||") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_logical_and_or(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("logical operators require bool operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_bitwise(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("&") || operation == mlc::String("|") || operation == mlc::String("^") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("bitwise ") + operation + mlc::String(" requires i32 operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_diagnostic_for_shift(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
return operation == mlc::String("<<") || operation == mlc::String(">>") ? !semantic_type_structure::type_is_unknown(left_type) && !semantic_type_structure::type_is_unknown(right_type) && !semantic_type_structure::types_allowed_for_binary_int_only(left_type, right_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("shift requires i32 operands, got ") + semantic_type_structure::type_description(left_type) + mlc::String(" and ") + semantic_type_structure::type_description(right_type), source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept{
mlc::Array<ast::Diagnostic> from_addition = infer_binary_diagnostic_for_addition(operation, left_type, right_type, source_span);
return from_addition.size() > 0 ? from_addition : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_int_arithmetic = infer_binary_diagnostic_for_int_arithmetic(operation, left_type, right_type, source_span);
  return from_int_arithmetic.size() > 0 ? from_int_arithmetic : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_equality = infer_binary_diagnostic_for_equality(operation, left_type, right_type, source_span);
  return from_equality.size() > 0 ? from_equality : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_ordering = infer_binary_diagnostic_for_ordering(operation, left_type, right_type, source_span);
  return from_ordering.size() > 0 ? from_ordering : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_logical = infer_binary_diagnostic_for_logical(operation, left_type, right_type, source_span);
  return from_logical.size() > 0 ? from_logical : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_bitwise = infer_binary_diagnostic_for_bitwise(operation, left_type, right_type, source_span);
  return from_bitwise.size() > 0 ? from_bitwise : infer_binary_diagnostic_for_shift(operation, left_type, right_type, source_span);
 }();
 }();
 }();
 }();
 }();
}

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
return operation == mlc::String("-") && !semantic_type_structure::type_is_unknown(inner_type) && !semantic_type_structure::type_is_i32(inner_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("unary - expects i32, got ") + semantic_type_structure::type_description(inner_type), source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
return operation == mlc::String("!") && !semantic_type_structure::type_is_unknown(inner_type) && !semantic_type_structure::type_is_bool(inner_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("operator ! expects bool, got ") + semantic_type_structure::type_description(inner_type), source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept{
return expected_length != actual_length ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected_length) + mlc::String(" arguments, got ") + mlc::to_string(actual_length), call_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_push(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("push") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method push expects an array receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_set(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("set") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method set expects an array or Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_length_or_size(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("length") || method_name == mlc::String("size") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("methods length and size apply to string or array; receiver is ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_to_integer_string_only(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("to_i") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method to_i expects a string receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_string_only_methods(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("char_at") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_string(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a string receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_join(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("join") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method join expects an array receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_has(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("has") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method has expects a Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_method_receiver_get_remove_keys_values(mlc::String method_name, std::shared_ptr<registry::Type> receiver_type, ast::Span method_span) noexcept{
return method_name == mlc::String("get") || method_name == mlc::String("remove") || method_name == mlc::String("keys") || method_name == mlc::String("values") ? !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::receiver_type_is_map(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("method ") + method_name + mlc::String(" expects a Map receiver, got ") + semantic_type_structure::type_description(receiver_type), method_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept{
mlc::Array<ast::Diagnostic> from_push = infer_method_receiver_push(method_name, receiver_type, method_span);
return from_push.size() > 0 ? from_push : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_set = infer_method_receiver_set(method_name, receiver_type, method_span);
  return from_set.size() > 0 ? from_set : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_length = infer_method_receiver_length_or_size(method_name, receiver_type, method_span);
  return from_length.size() > 0 ? from_length : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_to_i = infer_method_receiver_to_integer_string_only(method_name, receiver_type, method_span);
  return from_to_i.size() > 0 ? from_to_i : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_string_ops = infer_method_receiver_string_only_methods(method_name, receiver_type, method_span);
  return from_string_ops.size() > 0 ? from_string_ops : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_join = infer_method_receiver_join(method_name, receiver_type, method_span);
  return from_join.size() > 0 ? from_join : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> from_map_access = infer_method_receiver_get_remove_keys_values(method_name, receiver_type, method_span);
  return from_map_access.size() > 0 ? from_map_access : infer_method_receiver_has(method_name, receiver_type, method_span);
 }();
 }();
 }();
 }();
 }();
 }();
}

mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
int expected = semantic_type_structure::builtin_method_expected_argument_count(method_name);
return expected < 0 ? mlc::Array<ast::Diagnostic>{} : argument_count != expected ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected) + mlc::String(" arguments, got ") + mlc::to_string(argument_count), method_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept{
return expected_count != actual_count ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected_count) + mlc::String(" arguments, got ") + mlc::to_string(actual_count), call_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
return receiver_errors.size() > 0 ? mlc::Array<ast::Diagnostic>{} : infer_builtin_method_arity_diagnostics(method_name, argument_count, method_span);
}

mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept{
return !semantic_type_structure::type_is_unknown(receiver_type) && !semantic_type_structure::type_is_array(receiver_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("indexing requires an array, got ") + semantic_type_structure::type_description(receiver_type), bracket_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept{
return !semantic_type_structure::type_is_unknown(index_type) && !semantic_type_structure::type_is_i32(index_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("array index must be i32, got ") + semantic_type_structure::type_description(index_type), bracket_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept{
return branches_mismatch ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("if branch types differ: then ") + semantic_type_structure::type_description(then_type) + mlc::String(", else ") + semantic_type_structure::type_description(else_type), ast::expr_span(else_expression))} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept{
return !semantic_type_structure::type_is_unknown(iterator_type) && !semantic_type_structure::type_is_array(iterator_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("for loop range must be an array, got ") + semantic_type_structure::type_description(iterator_type), ast::expr_span(iterator))} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept{
return arm_index > 0 && !semantic_type_structure::type_is_unknown(first_arm_type) && !semantic_type_structure::type_is_unknown(arm_result_type) && !semantic_type_structure::types_structurally_equal(first_arm_type, arm_result_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match arm result type ") + semantic_type_structure::type_description(arm_result_type) + mlc::String(" does not match first arm type ") + semantic_type_structure::type_description(first_arm_type), ast::expr_span(arm_body))} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*object_type))) { auto _v_tnamed = std::get<registry::TNamed>((*object_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && registry::TypeRegistry_has_fields(registry, type_name) ? !registry::TypeRegistry_fields_for(registry, type_name).has(field_name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("undefined field: ") + field_name + mlc::String(" on ") + type_name, field_source_span)} : mlc::Array<ast::Diagnostic>{} : !semantic_type_structure::type_is_unknown(object_type) ? type_name != mlc::String("") && !registry::TypeRegistry_has_fields(registry, type_name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("field access needs a record type; ") + type_name + mlc::String(" is not one"), field_source_span)} : semantic_type_structure::type_is_array(object_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("array value has no record fields for .") + field_name, field_source_span)} : !semantic_type_structure::type_is_function(object_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("field access on value without record fields: ") + semantic_type_structure::type_description(object_type), field_source_span)} : mlc::Array<ast::Diagnostic>{} : mlc::Array<ast::Diagnostic>{};
}

} // namespace type_diagnostics
