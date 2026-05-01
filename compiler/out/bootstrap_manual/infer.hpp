#ifndef INFER_HPP
#define INFER_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_literals.hpp"
#include "infer_for_support.hpp"
#include "infer_question_expression.hpp"
#include "infer_operand_combine.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"
#include "infer_call.hpp"
#include "registry.hpp"
#include "infer_expr_ident.hpp"
#include "pattern_env.hpp"
#include "infer_match.hpp"
#include "let_pattern_infer.hpp"
#include "infer_array_method.hpp"
#include "infer_result_option_method.hpp"

namespace infer {

infer_result::InferResult infer_arguments_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::Expr>> expressions, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_field_values_errors(infer_result::InferResult initial, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, mlc::String record_type_name_for_fields) noexcept;
infer_result::InferResult infer_expr_binary(mlc::String operation, std::shared_ptr<ast::Expr> left, std::shared_ptr<ast::Expr> right, ast::Span source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_unary(mlc::String operation, std::shared_ptr<ast::Expr> inner, ast::Span source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_method(std::shared_ptr<ast::Expr> object, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
infer_result::InferResult infer_expr_field(std::shared_ptr<ast::Expr> object, mlc::String field_name, ast::Span field_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_index(std::shared_ptr<ast::Expr> object, std::shared_ptr<ast::Expr> index_expression, ast::Span bracket_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_conditional(std::shared_ptr<ast::Expr> condition, std::shared_ptr<ast::Expr> then_expression, std::shared_ptr<ast::Expr> else_expression, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_with(std::shared_ptr<ast::Expr> resource, mlc::String binder, mlc::Array<std::shared_ptr<ast::Stmt>> stmts, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_while_loop(std::shared_ptr<ast::Expr> condition, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_for_loop(mlc::String variable_name, std::shared_ptr<ast::Expr> iterator, mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context, ast::Span span) noexcept;
infer_result::InferResult infer_expr_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_tuple_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_array_literal(mlc::Array<std::shared_ptr<ast::Expr>> elements, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_question(std::shared_ptr<ast::Expr> inner, ast::Span question_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_lambda(mlc::Array<mlc::String> parameter_names, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr(std::shared_ptr<ast::Expr> expression, check_context::CheckContext inference_context) noexcept;
infer_result::StmtInferResult infer_statements(mlc::Array<std::shared_ptr<ast::Stmt>> statements, check_context::CheckContext inference_context) noexcept;

} // namespace infer

#endif // INFER_HPP
