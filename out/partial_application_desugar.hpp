#ifndef PARTIAL_APPLICATION_DESUGAR_HPP
#define PARTIAL_APPLICATION_DESUGAR_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace partial_application_desugar {

struct PartialPair {std::shared_ptr<ast::Expr> expression;int next_serial;};
struct StatementsPartialPair {mlc::Array<std::shared_ptr<ast::Stmt>> statements;int next_serial;};
struct StatementsBlockAccumulator {mlc::Array<std::shared_ptr<ast::Stmt>> accumulated_statements;int next_serial;};
struct ArgumentsPartialPair {mlc::Array<std::shared_ptr<ast::Expr>> arguments;int next_serial;};
struct ExpressionListAccumulator {mlc::Array<std::shared_ptr<ast::Expr>> mapped_expressions;int next_serial;};
struct RecordLitPartsPartialPair {mlc::Array<ast::RecordLitPart> parts;int next_serial;};
struct FieldValuesAccumulator {mlc::Array<std::shared_ptr<ast::FieldVal>> mapped_field_values;int next_serial;};
struct PartialNamesAllocation {mlc::Array<mlc::String> names;int next_serial;};
struct StmtPartialPair {std::shared_ptr<ast::Stmt> statement;int next_serial;};
struct MatchArmsPartialPair {mlc::Array<std::shared_ptr<ast::MatchArm>> arms;int next_serial;};
PartialPair partial_application_leaf_partial_pair(std::shared_ptr<ast::Expr> leaf_expression, int serial) noexcept;
bool expr_is_placeholder(std::shared_ptr<ast::Expr> expression) noexcept;
int partial_placeholder_delta_for_argument(std::shared_ptr<ast::Expr> argument) noexcept;
PartialNamesAllocation partial_allocate_parameter_names(int hole_count, int serial_start) noexcept;
int partial_count_placeholder_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;
ExpressionListAccumulator partial_application_desugar_expression_element_sequence(mlc::Array<std::shared_ptr<ast::Expr>> elements, int initial_serial) noexcept;
FieldValuesAccumulator partial_application_desugar_field_values_sequence(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int initial_serial) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments_inner(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span, int argument_index, int name_index, mlc::Array<std::shared_ptr<ast::Expr>> output) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> partial_replace_call_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, ast::Span fallback_span) noexcept;
PartialPair partial_wrap_call_after_children(std::shared_ptr<ast::Expr> callee, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept;
PartialPair partial_wrap_method_after_children(std::shared_ptr<ast::Expr> object_expression, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> mapped_arguments, ast::Span source_span, int serial) noexcept;
ArgumentsPartialPair partial_application_map_arguments_at(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int index, int serial, mlc::Array<std::shared_ptr<ast::Expr>> mapped) noexcept;
ArgumentsPartialPair partial_application_map_arguments(mlc::Array<std::shared_ptr<ast::Expr>> arguments, int serial) noexcept;
RecordLitPartsPartialPair partial_application_desugar_record_lit_parts(mlc::Array<ast::RecordLitPart> parts, int serial) noexcept;
StatementsPartialPair partial_application_desugar_statements_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, int serial) noexcept;
StmtPartialPair partial_application_desugar_statement(std::shared_ptr<ast::Stmt> statement, int serial) noexcept;
MatchArmsPartialPair partial_application_desugar_match_arms_list(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, int serial) noexcept;
PartialPair partial_application_desugar_inner(std::shared_ptr<ast::Expr> expression, int serial) noexcept;
std::shared_ptr<ast::Expr> partial_application_desugar_expr(std::shared_ptr<ast::Expr> expression) noexcept;

} // namespace partial_application_desugar

#endif // PARTIAL_APPLICATION_DESUGAR_HPP
