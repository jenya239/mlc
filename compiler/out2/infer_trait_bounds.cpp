#include "infer_trait_bounds.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "call_argument_unify.hpp"

namespace infer_trait_bounds {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace call_argument_unify;
using namespace ast_tokens;

mlc::String type_name_for_bound_check(std::shared_ptr<registry::Type> type_value) noexcept;

mlc::Array<ast::Diagnostic> check_one_type_param_bounds(mlc::String type_param, mlc::Array<mlc::String> bounds, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

mlc::Array<ast::Diagnostic> trait_bound_diagnostics(mlc::String callee_name, mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<mlc::String> callee_type_parameter_names, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

mlc::String type_name_for_bound_check(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<registry::TI32>((*type_value))) {  return mlc::String("i32"); } if (std::holds_alternative<registry::TString>((*type_value))) {  return mlc::String("string"); } if (std::holds_alternative<registry::TBool>((*type_value))) {  return mlc::String("bool"); } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();}

mlc::Array<ast::Diagnostic> check_one_type_param_bounds(mlc::String type_param, mlc::Array<mlc::String> bounds, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<ast::Diagnostic> param_errors = {};
if (substitution.has(type_param)){
{
std::shared_ptr<registry::Type> actual_type = substitution.get(type_param);
mlc::String actual_name = type_name_for_bound_check(actual_type);
if (actual_name != mlc::String("")){
int trait_index = 0;
[&]() { 
  while (trait_index < bounds.size()){
{
mlc::String required_trait = bounds[trait_index];
if (!registry::TypeRegistry_type_implements_trait(inference_context.registry, actual_name, required_trait)){
{
param_errors = ast::diagnostics_append(param_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("type ") + actual_name + mlc::String(" does not implement ") + required_trait, call_source_span)});
}
}
trait_index = trait_index + 1;
}
}
 }();
}
}
}
return param_errors;
}

mlc::Array<ast::Diagnostic> trait_bound_diagnostics(mlc::String callee_name, mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<mlc::String> callee_type_parameter_names, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<mlc::Array<mlc::String>> bounds_per_param = registry::TypeRegistry_fn_trait_bounds(inference_context.registry, callee_name);
return bounds_per_param.size() == 0 ? [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::Array<ast::Diagnostic> empty_bound_errors = {};
  return empty_bound_errors;
 }() : [&]() -> mlc::Array<ast::Diagnostic> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution = call_argument_unify::build_call_type_substitution(callee_parameter_types, argument_inferred_types, callee_type_parameter_names);
  mlc::Array<ast::Diagnostic> bound_errors = {};
  int param_index = 0;
  while (param_index < callee_type_parameter_names.size() && param_index < bounds_per_param.size()){
{
mlc::String type_param = callee_type_parameter_names[param_index];
mlc::Array<mlc::String> bounds = bounds_per_param[param_index];
bound_errors = ast::diagnostics_append(bound_errors, check_one_type_param_bounds(type_param, bounds, substitution, call_source_span, inference_context));
param_index = param_index + 1;
}
}
  return bound_errors;
 }();
}

} // namespace infer_trait_bounds
