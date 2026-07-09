#ifndef EXPRESSION_SUPPORT_HPP
#define EXPRESSION_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
namespace expression_support {

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;
std::shared_ptr<semantic_ir::SemanticExpression> find_field_value(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, mlc::String field_name) noexcept;
bool is_constructor_call(std::shared_ptr<semantic_ir::SemanticExpression> function_expr) noexcept;
mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;
mlc::String infer_shared_new_type_name(std::shared_ptr<semantic_ir::SemanticExpression> argument, context::CodegenContext context) noexcept;
mlc::String semantic_type_to_cpp_type_name(std::shared_ptr<registry::Type> type_value, context::CodegenContext context) noexcept;
mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;
mlc::String cpp_function_name_for_profile_method(mlc::String method_name) noexcept;
mlc::String field_access_operator(std::shared_ptr<semantic_ir::SemanticExpression> object, context::CodegenContext context) noexcept;
mlc::String generate_conditional_else_with_empty_array_coercion(std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> preferred_array_semantic_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SemanticExpression>, context::CodegenContext)> evaluate_expression) noexcept;

} // namespace expression_support

#endif // EXPRESSION_SUPPORT_HPP
