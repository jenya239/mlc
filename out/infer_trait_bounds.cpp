#define main mlc_user_main
#include "infer_trait_bounds.hpp"

namespace infer_trait_bounds {

mlc::String type_name_for_bound_check(std::shared_ptr<registry::Type> type_value) noexcept{
return std::visit(overloaded{[&](const registry::TI32& tI32) { return mlc::String("i32", 3); },
[&](const registry::TString& tString) { return mlc::String("string", 6); },
[&](const registry::TBool& tBool) { return mlc::String("bool", 4); },
[&](const registry::TNamed& tNamed) { auto [name] = tNamed; return name; },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*type_value));
}
mlc::Array<ast::Diagnostic> check_one_type_param_bounds(mlc::String type_param, mlc::Array<mlc::String> bounds, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
auto param_errors = mlc::Array<ast::Diagnostic>{};
if (substitution.has(type_param)) {
auto actual_type = substitution.get(type_param);
auto actual_name = type_name_for_bound_check(actual_type);
if ((actual_name != mlc::String("", 0))) {
auto trait_index = 0;
[&]() {
while ((trait_index < bounds.length())) {
auto required_trait = bounds[trait_index];
if ((!registry::TypeRegistry_type_implements_trait(inference_context.registry, actual_name, required_trait))) {
param_errors = ast::diagnostics_append(param_errors, mlc::Array{ast::diagnostic_error((((mlc::String("type ", 5) + actual_name) + mlc::String(" does not implement ", 20)) + required_trait), call_source_span)});
}
trait_index = (trait_index + 1);
}
}();
}
}
return param_errors;
}
mlc::Array<ast::Diagnostic> trait_bound_diagnostics(mlc::String callee_name, mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<mlc::String> callee_type_parameter_names, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
auto bounds_per_param = registry::TypeRegistry_fn_trait_bounds(inference_context.registry, callee_name);
if ((bounds_per_param.length() == 0)) {
auto empty_bound_errors = mlc::Array<ast::Diagnostic>{};
return empty_bound_errors;
} else {
auto substitution = call_argument_unify::build_call_type_substitution(callee_parameter_types, argument_inferred_types, callee_type_parameter_names);
auto bound_errors = mlc::Array<ast::Diagnostic>{};
auto param_index = 0;
while (((param_index < callee_type_parameter_names.length()) && (param_index < bounds_per_param.length()))) {
auto type_param = callee_type_parameter_names[param_index];
auto bounds = bounds_per_param[param_index];
bound_errors = ast::diagnostics_append(bound_errors, check_one_type_param_bounds(type_param, bounds, substitution, call_source_span, inference_context));
param_index = (param_index + 1);
}
return bound_errors;
}
}

} // namespace infer_trait_bounds
