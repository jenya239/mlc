#ifndef RECORD_FIELD_DEFAULT_INITIALIZER_HPP
#define RECORD_FIELD_DEFAULT_INITIALIZER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "expr.hpp"
#include "cpp_naming.hpp"

namespace record_field_default_initializer {

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept;
bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::String record_field_default_identifier_cpp(mlc::String name, context::CodegenContext context) noexcept;
mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

} // namespace record_field_default_initializer

#endif // RECORD_FIELD_DEFAULT_INITIALIZER_HPP
