#ifndef EXPR_EVAL_HPP
#define EXPR_EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "type_gen.hpp"
#include "literals.hpp"
#include "identifiers.hpp"
#include "match_analysis.hpp"
#include "expression_support.hpp"
#include "statement_context.hpp"
#include "expr.hpp"

namespace expr_eval {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

} // namespace expr_eval

#endif // EXPR_EVAL_HPP
