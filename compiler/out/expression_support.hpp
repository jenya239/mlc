#ifndef EXPRESSION_SUPPORT_HPP
#define EXPRESSION_SUPPORT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"

namespace expression_support {

struct Expr;
struct Stmt;

mlc::String cpp_lambda_header_prefix(mlc::Array<mlc::String> parameters) noexcept;

std::shared_ptr<ast::Expr> find_field_value(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String field_name) noexcept;

bool is_constructor_call(std::shared_ptr<ast::Expr> function_expr) noexcept;

mlc::String resolve_object_code_in_self_context(mlc::String object_name, context::CodegenContext context) noexcept;

mlc::String infer_shared_new_type_name(std::shared_ptr<ast::Expr> argument, context::CodegenContext context) noexcept;

mlc::String cpp_function_name_for_file_method(mlc::String method_name) noexcept;

mlc::String field_access_operator(std::shared_ptr<ast::Expr> object, context::CodegenContext context) noexcept;

} // namespace expression_support

#endif // EXPRESSION_SUPPORT_HPP
