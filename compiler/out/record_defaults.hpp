#ifndef RECORD_DEFAULTS_HPP
#define RECORD_DEFAULTS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "expr.hpp"
#include "cpp_naming.hpp"

namespace record_defaults {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

} // namespace record_defaults

#endif // RECORD_DEFAULTS_HPP
