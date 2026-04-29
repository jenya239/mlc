#define main mlc_user_main
#include "infer_match.hpp"

namespace infer_match {

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
auto arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, arms[arm_index]->pat, inference_context.registry, substitution);
auto arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
auto arm_result = infer_expr_fn(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
arm_type = ((arm_index == 0) ? arm_result.inferred_type : arm_type);
auto arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_result.inferred_type, arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = (arm_index + 1);
}
return infer_result::InferResult{arm_type, collected_errors};
}

} // namespace infer_match
