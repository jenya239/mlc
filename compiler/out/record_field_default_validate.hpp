#ifndef RECORD_FIELD_DEFAULT_VALIDATE_HPP
#define RECORD_FIELD_DEFAULT_VALIDATE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace record_field_default_validate {

bool binary_operator_permitted_for_record_field_default_initializer(mlc::String operation) noexcept;
bool record_field_default_expression_is_static_initializer(std::shared_ptr<ast::Expr> expression) noexcept;

} // namespace record_field_default_validate

#endif // RECORD_FIELD_DEFAULT_VALIDATE_HPP
