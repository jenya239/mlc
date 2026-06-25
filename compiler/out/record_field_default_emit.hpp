#ifndef RECORD_FIELD_DEFAULT_EMIT_HPP
#define RECORD_FIELD_DEFAULT_EMIT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"
#include "literals.hpp"
#include "expr.hpp"
#include "cpp_naming.hpp"

namespace record_field_default_emit {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

} // namespace record_field_default_emit

#endif // RECORD_FIELD_DEFAULT_EMIT_HPP
