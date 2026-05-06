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

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

bool record_field_default_expression_acceptable_for_codegen(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::String record_field_default_expression_cpp_initializer(std::shared_ptr<ast::Expr> expression, context::CodegenContext context) noexcept;

} // namespace record_field_default_initializer

#endif // RECORD_FIELD_DEFAULT_INITIALIZER_HPP
