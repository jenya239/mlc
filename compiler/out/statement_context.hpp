#ifndef STATEMENT_CONTEXT_HPP
#define STATEMENT_CONTEXT_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"

namespace statement_context {

struct Expr;
struct Stmt;

context::CodegenContext stmts_final_ctx(mlc::Array<std::shared_ptr<ast::Stmt>> statements, context::CodegenContext context) noexcept;

} // namespace statement_context

#endif // STATEMENT_CONTEXT_HPP
