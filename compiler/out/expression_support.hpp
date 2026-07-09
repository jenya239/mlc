#ifndef EXPRESSION_SUPPORT_HPP
#define EXPRESSION_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "result_option_method_types.hpp"
#include "semantic_type_structure.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
namespace expression_support {

mlc::String named_or_described_error_type_name(std::shared_ptr<registry::Type> type_value) noexcept;
mlc::String question_from_converter_name(context::CodegenContext context, std::shared_ptr<registry::Type> inner_result_type) noexcept;
mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String field_name) noexcept;
bool is_constructor_call(std::shared_ptr<semantic_ir::SemanticExpression> function_expr) noexcept;
mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;
mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context) noexcept;
mlc::String semantic_type_to_cpp_type_name(std::shared_ptr<registry::Type> type_value, context::CodegenContext context) noexcept;
mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;
mlc::String cpp_function_name_for_profile_method(mlc::String method_name) noexcept;
mlc::String field_access_operator(std::shared_ptr<semantic_ir::SemanticExpression> object, context::CodegenContext context) noexcept;
template<typename __F4>
mlc::String generate_conditional_else_with_empty_array_coercion(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> preferred_array_semantic_type, context::CodegenContext context, __F4 evaluate_expression) noexcept;
template<typename __F4>
mlc::String generate_conditional_else_with_empty_array_coercion(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> preferred_array_semantic_type, context::CodegenContext context, __F4 evaluate_expression) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = else_expression;
if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*__match_subject))) {
const semantic_ir::SemanticExpressionArray& semanticExpressionArray = std::get<semantic_ir::SemanticExpressionArray>((*__match_subject));
auto [elements, __1, array_span] = semanticExpressionArray; return [&]() -> mlc::String {
  if ((elements.length() > 0))   {
    return evaluate_expression(else_expression, context);
  } else   {
    return [&]() -> mlc::String {
auto __match_subject = preferred_array_semantic_type;
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_from_preferred] = tArray; return evaluate_expression(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_from_preferred}), array_span}), context);
}
return [&]() -> mlc::String {
auto __match_subject = semantic_ir::sexpr_type(then_expression);
if (std::holds_alternative<registry::TArray>((*__match_subject))) {
const registry::TArray& tArray = std::get<registry::TArray>((*__match_subject));
auto [inner_then] = tArray; return evaluate_expression(std::make_shared<semantic_ir::SemanticExpression>(semantic_ir::SemanticExpressionArray{elements, std::make_shared<registry::Type>(registry::TArray{inner_then}), array_span}), context);
}
return evaluate_expression(else_expression, context);
std::abort();
}();
std::abort();
}();
  }
}();
}
return evaluate_expression(else_expression, context);
std::abort();
}();
}

} // namespace expression_support

#endif // EXPRESSION_SUPPORT_HPP
