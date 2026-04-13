#ifndef RETURN_BODY_HPP
#define RETURN_BODY_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "context.hpp"
#include "statement_context.hpp"
#include "expr.hpp"
#include "eval.hpp"

namespace return_body {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String gen_return_body(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context) noexcept;

mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SExpr> body_expr, context::CodegenContext context) noexcept;

} // namespace return_body

#endif // RETURN_BODY_HPP
