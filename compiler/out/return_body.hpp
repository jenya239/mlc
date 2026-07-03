#ifndef RETURN_BODY_HPP
#define RETURN_BODY_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
#include "semantic_ir.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "context.hpp"
#include "expr.hpp"
#include "eval.hpp"
#include "stmt_cpp.hpp"
#include "expr_visitor_cpp.hpp"
namespace return_body {

mlc::String gen_return_if_stmt(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;
mlc::String gen_return_body(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;
mlc::String gen_fn_body(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;
mlc::String gen_stmts_for_return_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppExpression> gen_expr_for_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> expression, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppExpression> try_result_identifier_expression(mlc::String try_identifier) noexcept;
std::shared_ptr<cpp_ast::CppExpression> try_result_address_expression(mlc::String try_identifier) noexcept;
std::shared_ptr<cpp_ast::CppExpression> try_result_err_pointer_expression(mlc::String try_identifier) noexcept;
std::shared_ptr<cpp_ast::CppExpression> try_result_ok_field0_expression(mlc::String try_identifier) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_try_unwrap_return_statements_cpp(std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_leaf_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppStatement> gen_return_body_branch_block(std::shared_ptr<semantic_ir::SemanticExpression> branch_expression, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppStatement> gen_return_body_else_branch(std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;
std::shared_ptr<cpp_ast::CppStatement> gen_return_if_cpp_statement(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_if_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_block_body_cpp(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> block_statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_match_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> match_arms, std::shared_ptr<registry::Type> match_type, ast::Span span, context::CodegenContext context) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> gen_return_body_cpp(std::shared_ptr<semantic_ir::SemanticExpression> body_expression, context::CodegenContext context) noexcept;

} // namespace return_body

#endif // RETURN_BODY_HPP
