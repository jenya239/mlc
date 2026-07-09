#ifndef INFER_CALL_HPP
#define INFER_CALL_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_call_support.hpp"
#include "call_argument_unify.hpp"
#include "infer_trait_bounds.hpp"
#include "semantic_type_structure.hpp"
#include "named_args.hpp"
namespace infer_call {

struct Call_arguments_inference_accumulator {
  mlc::Array<ast::Diagnostic> accumulated_errors;
  mlc::Array<std::shared_ptr<registry::Type>> inferred_argument_types_list;
};
template<typename __F3>
Call_arguments_inference_accumulator accumulate_inference_for_one_call_argument(Call_arguments_inference_accumulator accumulator, std::shared_ptr<ast::Expr> argument_expression, check_context::CheckContext inference_context, __F3 infer_expr_fn) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int positional_argument_count) noexcept;
mlc::String algebraic_type_name_from_ctor(registry::TypeRegistry registry, mlc::String callee_name) noexcept;
mlc::String callee_name_from_expr(std::shared_ptr<ast::Expr> function) noexcept;
mlc::Array<std::shared_ptr<registry::Type>> generic_type_arguments_from_substitution(mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;
std::shared_ptr<registry::Type> instantiated_algebraic_constructor_type(mlc::String owner_name, mlc::Array<mlc::String> type_parameter_names, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution) noexcept;
infer_result::InferResult infer_expr_call_constructor_path(mlc::String callee_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;
int callee_min_arity(mlc::String callee_name, int max_arity, registry::TypeRegistry registry) noexcept;
mlc::Array<mlc::String> registered_function_type_parameter_names_for_callee(mlc::String callee_name, registry::TypeRegistry registry) noexcept;
infer_result::InferResult infer_expr_call_non_constructor_path(mlc::String callee_name, infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;
infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;
template<typename __F3>
Call_arguments_inference_accumulator accumulate_inference_for_one_call_argument(Call_arguments_inference_accumulator accumulator, std::shared_ptr<ast::Expr> argument_expression, check_context::CheckContext inference_context, __F3 infer_expr_fn) noexcept{
  auto inference_for_single_argument = infer_expr_fn(argument_expression, inference_context);
  return Call_arguments_inference_accumulator{ast::diagnostics_append(accumulator.accumulated_errors, inference_for_single_argument.errors), accumulator.inferred_argument_types_list.concat(mlc::Array<std::shared_ptr<registry::Type>>{inference_for_single_argument.inferred_type})};
}

} // namespace infer_call

#endif // INFER_CALL_HPP
