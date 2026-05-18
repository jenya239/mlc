#define main mlc_user_main
#include "infer_match.hpp"

namespace infer_match {

mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pat> pattern) noexcept{
return std::visit(overloaded{[&](const ast::PatTuple& patTuple) { auto [__0, span] = patTuple; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard is not supported with tuple patterns", 48), span)}; },
[&](const ast::PatArray& patArray) { auto [__0, __1, span] = patArray; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard is not supported with array patterns", 48), span)}; },
[&](const auto& __v) { return [&]() {
auto empty_list = mlc::Array<ast::Diagnostic>{};
return empty_list;
}(); }
}, (*pattern));
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept{
auto substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
std::visit(overloaded{[&](const registry::TGeneric& tGeneric) { auto [type_name, type_args] = tGeneric; return [&]() {
auto param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(context.registry, type_name);
auto i = 0;
return [&]() {
while (((i < param_names.length()) && (i < type_args.length()))) {
substitution.set(param_names[i], type_args[i]);
i = (i + 1);
}
}();
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*subject_type));
return substitution;
}
infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
auto subject_result = infer_expr_fn(subject, inference_context);
auto substitution = build_match_substitution(subject_result.inferred_type, inference_context);
auto collected_errors = subject_result.errors;
auto arm_type = std::make_shared<registry::Type>(registry::TUnknown{});
auto arm_index = 0;
while ((arm_index < arms.length())) {
auto arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, arms[arm_index]->pat, inference_context.registry, substitution, subject_result.inferred_type);
auto arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
if (arms[arm_index]->has_guard) {
collected_errors = ast::diagnostics_append(collected_errors, pattern_guard_unsupported_tuple_or_array_diagnostic(arms[arm_index]->pat));
auto guard_inference = infer_expr_fn(arms[arm_index]->when_condition, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, guard_inference.errors);
if (((!semantic_type_structure::type_is_unknown(guard_inference.inferred_type)) && (!semantic_type_structure::type_is_bool(guard_inference.inferred_type)))) {
collected_errors = ast::diagnostics_append(collected_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard must have type bool", 31), ast::expr_span(arms[arm_index]->when_condition))});
}
}
auto arm_result = infer_expr_fn(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
arm_type = ((arm_index == 0) ? (arm_result.inferred_type) : (arm_type));
auto arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_result.inferred_type, arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = (arm_index + 1);
}
return infer_result::InferResult{arm_type, collected_errors};
}

} // namespace infer_match
