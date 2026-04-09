#ifndef TYPE_DIAGNOSTICS_HPP
#define TYPE_DIAGNOSTICS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "type_utils.hpp"

namespace type_diagnostics {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

mlc::Array<ast::Diagnostic> infer_binary_operand_diagnostics(mlc::String operation, std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> unary_minus_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> unary_bang_diagnostic(mlc::String operation, std::shared_ptr<registry::Type> inner_type, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> constructor_arity_diagnostics(int expected_length, int actual_length, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> infer_builtin_method_receiver_diagnostics(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> call_arity_diagnostics(int expected_count, int actual_count, ast::Span call_source_span) noexcept;

mlc::Array<ast::Diagnostic> call_positional_argument_type_diagnostics(mlc::Array<std::shared_ptr<registry::Type>> parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions) noexcept;

mlc::Array<ast::Diagnostic> method_arity_after_receiver(mlc::Array<ast::Diagnostic> receiver_errors, mlc::String method_name, int argument_count, ast::Span method_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_array_diagnostic(std::shared_ptr<registry::Type> receiver_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> index_not_i32_diagnostic(std::shared_ptr<registry::Type> index_type, ast::Span bracket_source_span) noexcept;

mlc::Array<ast::Diagnostic> if_branch_mismatch_diagnostic(bool branches_mismatch, std::shared_ptr<registry::Type> then_type, std::shared_ptr<registry::Type> else_type, std::shared_ptr<ast::Expr> else_expression) noexcept;

mlc::Array<ast::Diagnostic> for_loop_range_diagnostic(std::shared_ptr<registry::Type> iterator_type, std::shared_ptr<ast::Expr> iterator) noexcept;

mlc::Array<ast::Diagnostic> match_arm_type_mismatch_diagnostic(int arm_index, std::shared_ptr<registry::Type> first_arm_type, std::shared_ptr<registry::Type> arm_result_type, std::shared_ptr<ast::Expr> arm_body) noexcept;

mlc::Array<ast::Diagnostic> infer_expr_field_diagnostics(std::shared_ptr<registry::Type> object_type, mlc::String field_name, ast::Span field_source_span, registry::TypeRegistry registry) noexcept;

} // namespace type_diagnostics

#endif // TYPE_DIAGNOSTICS_HPP
