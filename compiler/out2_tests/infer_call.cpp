#define main mlc_user_main
#include "infer_call.hpp"

namespace infer_call {

void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
mlc::Array<std::shared_ptr<registry::Type>> take_first_param_types(mlc::Array<std::shared_ptr<registry::Type>> param_types, int n) noexcept{
auto out = mlc::Array<std::shared_ptr<registry::Type>>{};
auto j = 0;
while (((j < n) && (j < param_types.length()))) {
out.push_back(param_types[j]);
j = (j + 1);
}
return out;
}
infer_result::InferResult infer_expr_call(std::shared_ptr<ast::Expr> function, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept{
auto function_result = infer_expr_fn(function, inference_context);
auto collected_errors = function_result.errors;
auto argument_inferred_types = mlc::Array<std::shared_ptr<registry::Type>>{};
auto argument_index = 0;
while ((argument_index < call_arguments.length())) {
auto argument_inference = infer_expr_fn(call_arguments[argument_index], inference_context);
collected_errors = ast::diagnostics_append(collected_errors, argument_inference.errors);
argument_inferred_types.push_back(argument_inference.inferred_type);
argument_index = (argument_index + 1);
}
auto with_arguments = infer_result::InferResult{function_result.inferred_type, collected_errors};
auto callee_name = std::visit(overloaded{[&](const ast::ExprIdent& exprIdent) { auto [name, __1] = exprIdent; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*function));
if (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_ctor(inference_context.registry, callee_name))) {
auto constructor_output = infer_call_support::infer_expr_call_for_constructor_name(callee_name, with_arguments, call_arguments, call_source_span, inference_context);
auto constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, callee_name);
auto algebraic_type_name = std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [parent_name] = tNamed; return parent_name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*registry::TypeRegistry_ctor_type(inference_context.registry, callee_name)));
auto constructor_type_parameter_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(inference_context.registry, algebraic_type_name);
if ((constructor_parameter_types.length() == call_arguments.length())) {
return infer_result::InferResult{constructor_output.inferred_type, ast::diagnostics_append(constructor_output.errors, call_argument_unify::call_argument_unification_diagnostics(constructor_parameter_types, argument_inferred_types, constructor_type_parameter_names, call_arguments))};
} else {
return constructor_output;
}
} else {
auto non_constructor_output = infer_call_support::infer_expr_call_non_constructor(function_result, with_arguments, call_arguments, call_source_span, inference_context.registry, callee_name);
auto callee_parameter_types = semantic_type_structure::function_parameter_list(function_result.inferred_type);
auto max_arity = callee_parameter_types.length();
auto min_arity = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_fn(inference_context.registry, callee_name)) ? [&]() {
auto r = registry::TypeRegistry_required_arity_for_fn(inference_context.registry, callee_name);
return ((r >= 0) ? r : max_arity);
}() : max_arity);
auto got = call_arguments.length();
auto callee_type_parameter_names = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_fn(inference_context.registry, callee_name)) ? registry::TypeRegistry_registered_function_type_parameter_names(inference_context.registry, callee_name) : [&]() {
auto empty_callee_type_names = mlc::Array<mlc::String>{};
return empty_callee_type_names;
}());
if (((semantic_type_structure::type_is_function(function_result.inferred_type) && (got >= min_arity)) && (got <= max_arity))) {
auto params_for_unify = take_first_param_types(callee_parameter_types, got);
auto unification_errors = call_argument_unify::call_argument_unification_diagnostics(params_for_unify, argument_inferred_types, callee_type_parameter_names, call_arguments);
auto bound_errors = infer_trait_bounds::trait_bound_diagnostics(callee_name, params_for_unify, argument_inferred_types, callee_type_parameter_names, call_source_span, inference_context);
return infer_result::InferResult{non_constructor_output.inferred_type, ast::diagnostics_append(ast::diagnostics_append(non_constructor_output.errors, unification_errors), bound_errors)};
} else {
return non_constructor_output;
}
}
}

} // namespace infer_call
