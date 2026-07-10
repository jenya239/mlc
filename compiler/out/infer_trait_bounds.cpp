#define main mlc_user_main
#include "infer_trait_bounds.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "call_argument_unify.hpp"
#include "diagnostic_codes.hpp"

namespace infer_trait_bounds {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace call_argument_unify;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::String type_name_for_bound_check(std::shared_ptr<registry::Type> type_value) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = type_value;
if (std::holds_alternative<registry::TI32>((*__match_subject))) {
const registry::TI32& tI32 = std::get<registry::TI32>((*__match_subject));
return mlc::String("i32", 3);
}
if (std::holds_alternative<registry::TString>((*__match_subject))) {
const registry::TString& tString = std::get<registry::TString>((*__match_subject));
return mlc::String("string", 6);
}
if (std::holds_alternative<registry::TBool>((*__match_subject))) {
const registry::TBool& tBool = std::get<registry::TBool>((*__match_subject));
return mlc::String("bool", 4);
}
if (std::holds_alternative<registry::TNamed>((*__match_subject))) {
const registry::TNamed& tNamed = std::get<registry::TNamed>((*__match_subject));
auto [name] = tNamed; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::Array<ast::Diagnostic> check_one_type_param_bounds(mlc::String type_param, mlc::Array<mlc::String> bounds, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> substitution, mlc::HashMap<mlc::String, int> type_parameter_source_argument_index, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
  auto param_errors = mlc::Array<ast::Diagnostic>{};
  if (substitution.has(type_param))   {
    auto actual_type = substitution.get(type_param);
    auto actual_name = type_name_for_bound_check(actual_type);
    if ((actual_name != mlc::String("", 0)))     {
      auto bound_diagnostic_span = call_argument_unify::type_parameter_source_argument_span(type_param, type_parameter_source_argument_index, argument_expressions, call_source_span);
      auto trait_index = 0;
      [&]() {
while ((trait_index < bounds.length())) {
auto required_trait = bounds[trait_index];
if ((!registry::TypeRegistry_type_implements_trait(inference_context.registry, actual_name, required_trait))) {
  (param_errors = ast::diagnostics_append(param_errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code((((mlc::String("type ", 5) + actual_name) + mlc::String(" does not implement ", 20)) + required_trait), bound_diagnostic_span, diagnostic_codes::diagnostic_code_e033())}));
}
(trait_index = mlc::arith::checked_add(trait_index, 1));
}
}();
    }
  }
  return param_errors;
}
mlc::Array<ast::Diagnostic> trait_bound_diagnostics(mlc::String callee_name, mlc::Array<std::shared_ptr<registry::Type>> callee_parameter_types, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types, mlc::Array<mlc::String> callee_type_parameter_names, ast::Span call_source_span, mlc::Array<std::shared_ptr<ast::Expr>> argument_expressions, check_context::CheckContext inference_context) noexcept{
  auto bounds_per_param = registry::TypeRegistry_fn_trait_bounds(inference_context.registry, callee_name);
  if ((bounds_per_param.length() == 0))   {
    auto empty_bound_errors = mlc::Array<ast::Diagnostic>{};
    return empty_bound_errors;
  } else   {
    auto type_parameter_source_argument_index = mlc::HashMap<mlc::String, int>();
    auto substitution = call_argument_unify::build_call_type_substitution(callee_parameter_types, argument_inferred_types, callee_type_parameter_names, type_parameter_source_argument_index, argument_expressions, call_source_span);
    auto bound_errors = mlc::Array<ast::Diagnostic>{};
    auto param_index = 0;
    while (((param_index < callee_type_parameter_names.length()) && (param_index < bounds_per_param.length())))     {
      auto type_param = callee_type_parameter_names[param_index];
      auto bounds = bounds_per_param[param_index];
      (bound_errors = ast::diagnostics_append(bound_errors, check_one_type_param_bounds(type_param, bounds, substitution, type_parameter_source_argument_index, argument_expressions, call_source_span, inference_context)));
      (param_index = mlc::arith::checked_add(param_index, 1));
    }
    return bound_errors;
  }
}

} // namespace infer_trait_bounds
