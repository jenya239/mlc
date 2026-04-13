#ifndef INFER_OPERAND_COMBINE_HPP
#define INFER_OPERAND_COMBINE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "infer_result.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "type_diagnostics.hpp"

namespace infer_operand_combine {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

infer_result::InferResult infer_binary_from_operand_results(mlc::String operation, infer_result::InferResult left_result, infer_result::InferResult right_result, ast::Span source_span) noexcept;

infer_result::InferResult infer_unary_from_inner_result(mlc::String operation, infer_result::InferResult inner_result, ast::Span source_span) noexcept;

infer_result::InferResult infer_index_from_operand_results(infer_result::InferResult object_result, infer_result::InferResult index_result, ast::Span bracket_source_span) noexcept;

infer_result::InferResult infer_conditional_from_branch_results(infer_result::InferResult condition_result, infer_result::InferResult then_result, infer_result::InferResult else_result, std::shared_ptr<ast::Expr> else_expression) noexcept;

infer_result::InferResult infer_field_from_object_result(infer_result::InferResult object_result, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_method_from_object_and_arguments(infer_result::InferResult object_result, infer_result::InferResult with_arguments, mlc::String method_name, ast::Span method_span, int argument_count) noexcept;

} // namespace infer_operand_combine

#endif // INFER_OPERAND_COMBINE_HPP
