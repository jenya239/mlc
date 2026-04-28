#include "infer_match.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "pattern_env.hpp"

namespace infer_match {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace type_diagnostics;
using namespace pattern_env;
using namespace ast_tokens;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept;

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>>();
[&]() -> void { if (std::holds_alternative<registry::TGeneric>((*subject_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*subject_type)); auto [type_name, type_args] = _v_tgeneric; return [&]() { 
  mlc::Array<mlc::String> param_names = registry::TypeRegistry_algebraic_decl_type_parameter_names_for(context.registry, type_name);
  int i = 0;
  return [&]() { 
  while (i < param_names.size() && i < type_args.size()){
{
substitution.set(param_names[i], type_args[i]);
i = i + 1;
}
}
 }();
 }(); } return; }();
return substitution;
}

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
infer_result::InferResult subject_result = infer_expr_fn(subject, inference_context);
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = build_match_substitution(subject_result.inferred_type, inference_context);
mlc::Array<ast::Diagnostic> collected_errors = subject_result.errors;
std::shared_ptr<registry::Type> arm_type = std::make_shared<registry::Type>((registry::TUnknown{}));
int arm_index = 0;
while (arm_index < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, arms[arm_index]->pat, inference_context.registry, substitution);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
infer_result::InferResult arm_result = infer_expr_fn(arms[arm_index]->body, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, arm_result.errors);
arm_type = arm_index == 0 ? arm_result.inferred_type : arm_type;
mlc::Array<ast::Diagnostic> arm_mismatch = type_diagnostics::match_arm_type_mismatch_diagnostic(arm_index, arm_type, arm_result.inferred_type, arms[arm_index]->body);
collected_errors = ast::diagnostics_append(collected_errors, arm_mismatch);
arm_index = arm_index + 1;
}
}
return infer_result::InferResult{arm_type, collected_errors};
}

} // namespace infer_match
