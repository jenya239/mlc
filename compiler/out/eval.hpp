#ifndef EVAL_HPP
#define EVAL_HPP

#include "mlc.hpp"
#include <variant>

#include "semantic_ir.hpp"
#include "context.hpp"
#include "print.hpp"
#include "stmt_eval.hpp"
#include "expr_visitor_cpp.hpp"
#include "stmt_cpp.hpp"

namespace eval {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String gen_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;

mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;

mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions, context::CodegenContext context) noexcept;

mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;

context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;

context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

} // namespace eval

#endif // EVAL_HPP
