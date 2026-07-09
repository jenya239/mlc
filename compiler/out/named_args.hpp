#ifndef NAMED_ARGS_HPP
#define NAMED_ARGS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "diagnostic_codes.hpp"
namespace named_args {

struct ResolvedNamedArguments {
  mlc::Array<std::shared_ptr<ast::Expr>> expressions;
  mlc::Array<ast::Diagnostic> errors;
};
struct Named_argument_reorder_state {
  mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions;
  mlc::Array<int> slot_filled_flags;
  mlc::Array<ast::Diagnostic> errors;
  int positional_slot;
};
struct Compact_slot_expressions_fold_state {
  mlc::Array<std::shared_ptr<ast::Expr>> compact_expressions;
  int next_slot_index;
};
bool expression_is_named_argument(std::shared_ptr<ast::Expr> argument) noexcept;
bool call_arguments_contain_named_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;
int find_parameter_slot(mlc::Array<mlc::String> parameter_names, mlc::String sought_parameter_name) noexcept;
std::shared_ptr<ast::Expr> inner_expression_without_named_label(std::shared_ptr<ast::Expr> argument) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> strip_named_argument_labels(mlc::Array<std::shared_ptr<ast::Expr>> arguments) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> build_placeholder_slot_expressions(mlc::Array<mlc::String> parameter_names) noexcept;
mlc::Array<int> build_zero_filled_slot_flags(mlc::Array<mlc::String> parameter_names) noexcept;
int first_unfilled_positional_slot(mlc::Array<int> slot_filled_flags, int parameter_count, int start_at) noexcept;
Named_argument_reorder_state process_one_call_argument_for_named_reorder(Named_argument_reorder_state state, std::shared_ptr<ast::Expr> argument, mlc::Array<mlc::String> parameter_names, int parameter_count, bool collect_parameter_errors) noexcept;
Compact_slot_expressions_fold_state compact_slot_expressions_fold_step(Compact_slot_expressions_fold_state state, std::shared_ptr<ast::Expr> slot_expression, mlc::Array<int> slot_filled_flags) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> compact_filled_slot_expressions(mlc::Array<std::shared_ptr<ast::Expr>> slot_expressions, mlc::Array<int> slot_filled_flags) noexcept;
ResolvedNamedArguments reorder_named_arguments_to_positional(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::Array<mlc::String> parameter_names, bool collect_parameter_errors) noexcept;
ResolvedNamedArguments resolve_named_call_arguments_for_infer(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;
mlc::Array<std::shared_ptr<ast::Expr>> resolve_named_call_arguments_for_transform(mlc::Array<std::shared_ptr<ast::Expr>> arguments, mlc::String callee_name, registry::TypeRegistry registry) noexcept;

} // namespace named_args

#endif // NAMED_ARGS_HPP
