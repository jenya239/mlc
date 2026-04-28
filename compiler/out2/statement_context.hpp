#ifndef STATEMENT_CONTEXT_HPP
#define STATEMENT_CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "context.hpp"

namespace statement_context {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<semantic_ir::SStmt>> statements, context::CodegenContext context) noexcept;

} // namespace statement_context

#endif // STATEMENT_CONTEXT_HPP
