#include "infer_call.hpp"

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

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace infer_call_support;
using namespace call_argument_unify;
using namespace infer_trait_bounds;
using namespace semantic_type_structure;
using namespace named_args;
using namespace ast_tokens;

struct Call_arguments_inference_accumulator {mlc::Array<ast::Diagnostic> accumulated_errors;mlc::Array<std::shared_ptr<registry::Type>> inferred_argument_types_list;};

infer_call::Call_arguments_inference_accumulator accumulate_inference_for_one_call_argument(infer_call::Call_arguments_inference_accumulator accumulator, std::shared_ptr<ast::Expr> argument_expression, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int positional_argument_count) noexcept;

mlc::String algebraic_type_name_from_ctor(registry::TypeRegistry registry, mlc::String callee_name) noexcept;

mlc::String callee_name_from_expr(std::shared_ptr<ast::Expr> function) noexcept;

infer_result::InferResult infer_expr_call_constructor_path(mlc::String callee_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

int callee_min_arity(mlc::String callee_name, int max_arity, registry::TypeRegistry registry) noexcept;

mlc::Array<mlc::String> registered_function_type_parameter_names_for_callee(mlc::String callee_name, registry::TypeRegistry registry) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_path(mlc::String callee_name, infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_call::Call_arguments_inference_accumulator accumulate_inference_for_one_call_argument(infer_call::Call_arguments_inference_accumulator accumulator, std::shared_ptr<ast::Expr> argument_expression, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult inference_for_single_argument = infer_expr_fn(argument_expression, inference_context);
return infer_call::Call_arguments_inference_accumulator{ast::diagnostics_append(accumulator.accumulated_errors, inference_for_single_argument.errors), accumulator.inferred_argument_types_list.concat(mlc::Array<std::shared_ptr<registry::Type>>{inference_for_single_argument.inferred_type})};
}

mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int positional_argument_count) noexcept{return param_types.take(positional_argument_count);}

mlc::String algebraic_type_name_from_ctor(registry::TypeRegistry registry, mlc::String callee_name) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*registry::TypeRegistry_ctor_type(registry, callee_name)))) { auto _v_tnamed = std::get<registry::TNamed>((*registry::TypeRegistry_ctor_type(registry, callee_name))); auto [parent_name] = _v_tnamed; return parent_name; } return mlc::String(""); }();}

mlc::String callee_name_from_expr(std::shared_ptr<ast::Expr> function) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();}

infer_result::InferResult infer_expr_call_constructor_path(mlc::String callee_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult constructor_output = infer_call_support::infer_expr_call_for_constructor_name(callee_name, with_arguments, effective_args, call_source_span, inference_context);
mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, callee_name);
mlc::String algebraic_type_name = algebraic_type_name_from_ctor(inference_context.registry, callee_name);
mlc::Array<mlc::String> constructor_type_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(inference_context.registry, algebraic_type_name);
if (constructor_parameter_types.size() != effective_args.size()){
{
return constructor_output;
}
}
return infer_result::InferResult{constructor_output.inferred_type, ast::diagnostics_append(constructor_output.errors, call_argument_unify::call_argument_unification_diagnostics(constructor_parameter_types, argument_inferred_types, constructor_type_parameter_names, effective_args))};
}

int callee_min_arity(mlc::String callee_name, int max_arity, registry::TypeRegistry registry) noexcept{
if (callee_name == mlc::String("") || !registry::TypeRegistry_has_fn(registry, callee_name)){
{
return max_arity;
}
}
int required_arity = registry::TypeRegistry_required_arity_for_fn(registry, callee_name);
if (required_arity >= 0){
{
return required_arity;
}
}
return max_arity;
}

mlc::Array<mlc::String> registered_function_type_parameter_names_for_callee(mlc::String callee_name, registry::TypeRegistry registry) noexcept{
if (callee_name != mlc::String("") && registry::TypeRegistry_has_fn(registry, callee_name)){
{
return registry::TypeRegistry_registered_function_type_parameter_names(registry, callee_name);
}
}
return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_callee_type_names = {};
  return empty_callee_type_names;
 }();
}

infer_result::InferResult infer_expr_call_non_constructor_path(mlc::String callee_name, infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> effective_args, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
infer_result::InferResult non_constructor_output = infer_call_support::infer_expr_call_non_constructor(function_parsed, with_arguments, effective_args, call_source_span, inference_context.registry, callee_name);
mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types = semantic_type_structure::function_parameter_list(function_parsed.inferred_type);
int max_arity = callee_parameter_types.size();
int min_arity = callee_min_arity(callee_name, max_arity, inference_context.registry);
int argument_count = effective_args.size();
mlc::Array<mlc::String> callee_type_parameter_names = registered_function_type_parameter_names_for_callee(callee_name, inference_context.registry);
if (!semantic_type_structure::type_is_function(function_parsed.inferred_type)){
{
return non_constructor_output;
}
}
if (argument_count < min_arity){
{
return non_constructor_output;
}
}
if (argument_count > max_arity){
{
return non_constructor_output;
}
}
mlc::Array<std::shared_ptr<registry::Type>> params_for_unify = take_first_param_types(callee_parameter_types, argument_count);
mlc::Array<ast::Diagnostic> unification_errors = call_argument_unify::call_argument_unification_diagnostics(params_for_unify, argument_inferred_types, callee_type_parameter_names, effective_args);
mlc::Array<ast::Diagnostic> bound_errors = infer_trait_bounds::trait_bound_diagnostics(callee_name, params_for_unify, argument_inferred_types, callee_type_parameter_names, call_source_span, effective_args, inference_context);
return infer_result::InferResult{non_constructor_output.inferred_type, ast::diagnostics_append(ast::diagnostics_append(non_constructor_output.errors, unification_errors), bound_errors)};
}

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::String callee_name = callee_name_from_expr(function);
named_args::ResolvedNamedArguments resolved = named_args::resolve_named_call_arguments_for_infer(call_arguments, callee_name, inference_context.registry);
mlc::Array<std::shared_ptr<ast::Expr>> effective_args = resolved.expressions;
infer_result::InferResult function_parsed = infer_expr_fn(function, inference_context);
mlc::Array<std::shared_ptr<registry::Type>> fresh_argument_types_for_accumulator = {};
infer_call::Call_arguments_inference_accumulator inference_accumulator_after_arguments = effective_args.fold(infer_call::Call_arguments_inference_accumulator{ast::diagnostics_append(function_parsed.errors, resolved.errors), fresh_argument_types_for_accumulator}, [inference_context, infer_expr_fn](infer_call::Call_arguments_inference_accumulator accumulator_so_far, std::shared_ptr<ast::Expr> argument_expression_under_inference) mutable { return accumulate_inference_for_one_call_argument(accumulator_so_far, argument_expression_under_inference, inference_context, infer_expr_fn); });
mlc::Array<ast::Diagnostic> collected_errors = inference_accumulator_after_arguments.accumulated_errors;
mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types = inference_accumulator_after_arguments.inferred_argument_types_list;
infer_result::InferResult with_arguments = infer_result::InferResult{function_parsed.inferred_type, collected_errors};
if (callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(inference_context.registry, callee_name)){
{
return infer_expr_call_constructor_path(callee_name, with_arguments, effective_args, argument_inferred_types, call_source_span, inference_context);
}
}
return infer_expr_call_non_constructor_path(callee_name, function_parsed, with_arguments, effective_args, argument_inferred_types, call_source_span, inference_context);
}

} // namespace infer_call
