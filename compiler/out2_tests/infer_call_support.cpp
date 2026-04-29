#define main mlc_user_main
#include "infer_call_support.hpp"

namespace infer_call_support {

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
auto constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
auto call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.length(), call_arguments.length(), call_source_span);
return infer_result::InferResult{registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name), ast::diagnostics_append(with_arguments.errors, call_errors)};
}
infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
auto parameter_types = semantic_type_structure::function_parameter_list(function_result.inferred_type);
auto max_arity = parameter_types.length();
auto min_arity = (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_fn(registry, callee_name)) ? [&]() {
auto r = registry::TypeRegistry_required_arity_for_fn(registry, callee_name);
return ((r >= 0) ? r : max_arity);
}() : max_arity);
auto arity_errors = ((min_arity == max_arity) ? type_diagnostics::call_arity_diagnostics(max_arity, call_arguments.length(), call_source_span) : type_diagnostics::call_arity_range_diagnostics(min_arity, max_arity, call_arguments.length(), call_source_span));
return infer_result::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}
infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept{
if (semantic_type_structure::type_is_unknown(function_result.inferred_type)) {
return base;
} else {
return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(base.errors, mlc::Array{ast::diagnostic_error(((mlc::String("called value is not a function (got ", 36) + semantic_type_structure::type_description(function_result.inferred_type)) + mlc::String(")", 1)), call_source_span)})};
}
}
infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
auto return_type = std::visit(overloaded{[&](const registry::TFn& tFn) { auto [__0, return_ty] = tFn; return return_ty; },
[&](const auto& __v) { return std::make_shared<registry::Type>(registry::TUnknown{}); }
}, (*function_result.inferred_type));
auto base = infer_result::InferResult{return_type, with_arguments.errors};
if (semantic_type_structure::type_is_function(function_result.inferred_type)) {
return infer_expr_call_non_constructor_arity_only(base, function_result, call_arguments, call_source_span, return_type, registry, callee_name);
} else {
return infer_expr_call_non_constructor_not_callable(base, function_result, call_source_span);
}
}

} // namespace infer_call_support
