#ifndef PARTIAL_APPLICATION_DESUGAR_HPP
#define PARTIAL_APPLICATION_DESUGAR_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace partial_application_desugar {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept;

} // namespace partial_application_desugar

#endif // PARTIAL_APPLICATION_DESUGAR_HPP
