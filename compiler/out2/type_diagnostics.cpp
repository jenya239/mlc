#include "type_diagnostics.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "array_method_types.hpp"

namespace type_diagnostics {

using namespace ast;
using namespace registry;
using namespace semantic_type_structure;
using namespace array_method_types;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> call_arity_range_diagnostics(int min_count, int max_count, int actual_count, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept;

mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept;

mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept;

mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept;

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
return operation == mlc::String("-") && !semantic_type_structure::type_is_unknown(inner_type) && !semantic_type_structure::type_is_i32(inner_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("unary - expects i32, got ") + semantic_type_structure::type_description(inner_type), source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept{
return operation == mlc::String("!") && !semantic_type_structure::type_is_unknown(inner_type) && !semantic_type_structure::type_is_bool(inner_type) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("operator ! expects bool, got ") + semantic_type_structure::type_description(inner_type), source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept{
return expected_length != actual_length ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected_length) + mlc::String(" arguments, got ") + mlc::to_string(actual_length), call_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> infer_builtin_method_arity_diagnostics(mlc::String method_name, int argument_count, ast::Span method_span) noexcept{
int expected = array_method_types::is_array_hof_method(method_name) ? array_method_types::array_hof_expected_argument_count(method_name) : semantic_type_structure::builtin_method_expected_argument_count(method_name);
return expected < 0 ? mlc::Array<ast::Diagnostic>{} : argument_count != expected ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected) + mlc::String(" arguments, got ") + mlc::to_string(argument_count), method_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept{
return expected_count != actual_count ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected ") + mlc::to_string(expected_count) + mlc::String(" arguments, got ") + mlc::to_string(actual_count), call_source_span)} : mlc::Array<ast::Diagnostic>{};
}

mlc::Array<ast::Diagnostic> call_arity_range_diagnostics(int min_count, int max_count, int actual_count, ast::Span call_source_span) noexcept{
return actual_count < min_count || actual_count > max_count ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("expected between ") + mlc::to_string(min_count) + mlc::String(" and ") + mlc::to_string(max_count) + mlc::String(" arguments, got ") + mlc::to_string(actual_count), call_source_span)} : mlc::Array<ast::Diagnostic>{};
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
