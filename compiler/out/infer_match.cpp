#include "infer_match.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "pattern_env.hpp"
#include "semantic_type_structure.hpp"

namespace infer_match {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace type_diagnostics;
using namespace pattern_env;
using namespace semantic_type_structure;
using namespace ast_tokens;

mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pat> pattern) noexcept;

mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> build_match_substitution(std::shared_ptr<registry::Type> subject_type, check_context::CheckContext context) noexcept;

infer_result::InferResult infer_expr_match(std::shared_ptr<ast::Expr> subject, mlc::Array<std::shared_ptr<ast::MatchArm>> arms, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

mlc::Array<ast::Diagnostic> pattern_guard_unsupported_tuple_or_array_diagnostic(std::shared_ptr<ast::Pat> pattern) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<ast::PatTuple>((*pattern))) { auto _v_pattuple = std::get<ast::PatTuple>((*pattern)); auto [_w0, span] = _v_pattuple; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard is not supported with tuple patterns"), span)}; } if (std::holds_alternative<ast::PatArray>((*pattern))) { auto _v_patarray = std::get<ast::PatArray>((*pattern)); auto [_w0, _w1, span] = _v_patarray; return mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard is not supported with array patterns"), span)}; } return [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_list = {};
  return empty_list;
 }(); }();}

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
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> arm_environment = pattern_env::env_for_pattern_substituted(inference_context.type_env, arms[arm_index]->pat, inference_context.registry, substitution, subject_result.inferred_type);
check_context::CheckContext arm_context = check_context::check_context_new(arm_environment, inference_context.registry);
if (arms[arm_index]->has_guard){
{
collected_errors = ast::diagnostics_append(collected_errors, pattern_guard_unsupported_tuple_or_array_diagnostic(arms[arm_index]->pat));
infer_result::InferResult guard_inference = infer_expr_fn(arms[arm_index]->when_condition, arm_context);
collected_errors = ast::diagnostics_append(collected_errors, guard_inference.errors);
if (!semantic_type_structure::type_is_unknown(guard_inference.inferred_type) && !semantic_type_structure::type_is_bool(guard_inference.inferred_type)){
collected_errors = ast::diagnostics_append(collected_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("match guard must have type bool"), ast::expr_span(arms[arm_index]->when_condition))});
}
}
}
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
