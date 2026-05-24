#include "expr_eval.hpp"

#include "semantic_ir.hpp"
#include "context.hpp"
#include "expr_visitor_string.hpp"

namespace expr_eval {

using namespace semantic_ir;
using namespace context;
using namespace expr_visitor_string;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept;

mlc::String eval_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SExpr>> expressions, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expressions.map([context, gen_stmts](std::shared_ptr<semantic_ir::SExpr> expression) mutable { return eval_expr(expression, context, gen_stmts); }).join(mlc::String(", "));}

mlc::String eval_expr(std::shared_ptr<semantic_ir::SExpr> expr, context::CodegenContext context, std::function<mlc::String(mlc::Array<std::shared_ptr<semantic_ir::SStmt>>, context::CodegenContext)> gen_stmts) noexcept{return expr_visitor_string::eval_expr_with_visitor(expr, context, gen_stmts, eval_expr);}

} // namespace expr_eval
