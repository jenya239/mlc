#ifndef PARAM_DESTRUCTURE_EXPAND_HPP
#define PARAM_DESTRUCTURE_EXPAND_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "diagnostic_codes.hpp"

namespace param_destructure_expand {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool parameter_binding_is_plain_identifier(std::shared_ptr<ast::Param> parameter) noexcept;

ast::Program expand_parameter_destructuring_in_program(ast::Program program) noexcept;

mlc::Array<ast::Diagnostic> extern_parameter_destructure_diagnostics(ast::Program program) noexcept;

} // namespace param_destructure_expand

#endif // PARAM_DESTRUCTURE_EXPAND_HPP
