#include "infer_call.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "infer_call_support.hpp"
#include "call_argument_unify.hpp"
#include "infer_trait_bounds.hpp"
#include "semantic_type_structure.hpp"

namespace infer_call {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace infer_call_support;
using namespace call_argument_unify;
using namespace infer_trait_bounds;
using namespace semantic_type_structure;
using namespace ast_tokens;

using InferExprFn = std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)>;

mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int n) noexcept;

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int n) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> out = {};
int j = 0;
while (j < n && j < param_types.size()){
{
out.push_back(param_types[j]);
j = j + 1;
}
}
return out;
}

infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult function_result = infer_expr_fn(function, inference_context);
mlc::Array<ast::Diagnostic> collected_errors = function_result.errors;
mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types = {};
int argument_index = 0;
while (argument_index < call_arguments.size()){
{
infer_result::InferResult argument_inference = infer_expr_fn(call_arguments[argument_index], inference_context);
collected_errors = ast::diagnostics_append(collected_errors, argument_inference.errors);
argument_inferred_types.push_back(argument_inference.inferred_type);
argument_index = argument_index + 1;
}
}
infer_result::InferResult with_arguments = infer_result::InferResult{function_result.inferred_type, collected_errors};
mlc::String callee_name = [&]() -> mlc::String { if (std::holds_alternative<ast::ExprIdent>((*function)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*function)._); auto [name, _w0] = _v_exprident; return name; } return mlc::String(""); }();
return callee_name != mlc::String("") && registry::TypeRegistry_has_ctor(inference_context.registry, callee_name) ? [&]() -> infer_result::InferResult { 
  infer_result::InferResult constructor_output = infer_call_support::infer_expr_call_for_constructor_name(callee_name, with_arguments, call_arguments, call_source_span, inference_context);
  mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, callee_name);
  mlc::String algebraic_type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*registry::TypeRegistry_ctor_type(inference_context.registry, callee_name)))) { auto _v_tnamed = std::get<registry::TNamed>((*registry::TypeRegistry_ctor_type(inference_context.registry, callee_name))); auto [parent_name] = _v_tnamed; return parent_name; } return mlc::String(""); }();
  mlc::Array<mlc::String> constructor_type_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(inference_context.registry, algebraic_type_name);
  return constructor_parameter_types.size() == call_arguments.size() ? infer_result::InferResult{constructor_output.inferred_type, ast::diagnostics_append(constructor_output.errors, call_argument_unify::call_argument_unification_diagnostics(constructor_parameter_types, argument_inferred_types, constructor_type_parameter_names, call_arguments))} : constructor_output;
 }() : [&]() -> infer_result::InferResult { 
  infer_result::InferResult non_constructor_output = infer_call_support::infer_expr_call_non_constructor(function_result, with_arguments, call_arguments, call_source_span, inference_context.registry, callee_name);
  mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types = semantic_type_structure::function_parameter_list(function_result.inferred_type);
  int max_arity = callee_parameter_types.size();
  int min_arity = callee_name != mlc::String("") && registry::TypeRegistry_has_fn(inference_context.registry, callee_name) ? [&]() -> int { 
  int r = registry::TypeRegistry_required_arity_for_fn(inference_context.registry, callee_name);
  return r >= 0 ? r : max_arity;
 }() : max_arity;
  int got = call_arguments.size();
  mlc::Array<mlc::String> callee_type_parameter_names = callee_name != mlc::String("") && registry::TypeRegistry_has_fn(inference_context.registry, callee_name) ? registry::TypeRegistry_registered_function_type_parameter_names(inference_context.registry, callee_name) : [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_callee_type_names = {};
  return empty_callee_type_names;
 }();
  return semantic_type_structure::type_is_function(function_result.inferred_type) && got >= min_arity && got <= max_arity ? [&]() -> infer_result::InferResult { 
  mlc::Array<std::shared_ptr<registry::Type>> params_for_unify = take_first_param_types(callee_parameter_types, got);
  mlc::Array<ast::Diagnostic> unification_errors = call_argument_unify::call_argument_unification_diagnostics(params_for_unify, argument_inferred_types, callee_type_parameter_names, call_arguments);
  mlc::Array<ast::Diagnostic> bound_errors = infer_trait_bounds::trait_bound_diagnostics(callee_name, params_for_unify, argument_inferred_types, callee_type_parameter_names, call_source_span, inference_context);
  return infer_result::InferResult{non_constructor_output.inferred_type, ast::diagnostics_append(ast::diagnostics_append(non_constructor_output.errors, unification_errors), bound_errors)};
 }() : non_constructor_output;
 }();
}

} // namespace infer_call
