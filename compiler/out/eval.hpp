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

mlc::String eval_expr_gen_stmts_callback(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext codegen_context) noexcept;
mlc::String CodegenContext_gen_stmts_str(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements) noexcept;
mlc::String CodegenContext_gen_expr(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;
mlc::String CodegenContext_gen_argument_list(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions) noexcept;
mlc::String CodegenContext_gen_try_unwrap(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, mlc::String try_identifier, mlc::String success_line) noexcept;
context::GenStmtResult CodegenContext_gen_stmt_with_try(context::CodegenContext self, std::shared_ptr<semantic_ir::SemanticStatement> statement, int try_counter) noexcept;
context::GenStmtsWithContext CodegenContext_gen_stmts_str_with_try(context::CodegenContext self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, int try_counter) noexcept;
mlc::String gen_expr(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;
mlc::String gen_argument_list(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> expressions, context::CodegenContext context) noexcept;
mlc::String gen_try_unwrap(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context, mlc::String try_identifier, mlc::String success_line) noexcept;
context::GenStmtResult gen_stmt_with_try(std::shared_ptr<semantic_ir::SemanticStatement> statement, context::CodegenContext context, int try_counter) noexcept;
context::GenStmtsWithContext gen_stmts_str_with_try(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context, int try_counter) noexcept;

} // namespace eval

#endif // EVAL_HPP
