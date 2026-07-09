#define main mlc_user_main
#include "infer_call_support.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "registry.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"
#include "diagnostic_codes.hpp"

namespace infer_call_support {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace registry;
using namespace infer_result;
using namespace type_diagnostics;
using namespace semantic_type_structure;
using namespace diagnostic_codes;
using namespace ast_tokens;

mlc::String owner_name_from_constructor_type(std::shared_ptr<registry::Type> constructor_type) noexcept{
  return std::visit(overloaded{[&](const registry::TNamed& tNamed) { auto [type_name] = tNamed; return type_name; },
[&](const registry::TI32& tI32) { return mlc::String("", 0); },
[&](const registry::TString& tString) { return mlc::String("", 0); },
[&](const registry::TBool& tBool) { return mlc::String("", 0); },
[&](const registry::TUnit& tUnit) { return mlc::String("", 0); },
[&](const registry::TI64& tI64) { return mlc::String("", 0); },
[&](const registry::TF64& tF64) { return mlc::String("", 0); },
[&](const registry::TU8& tU8) { return mlc::String("", 0); },
[&](const registry::TUsize& tUsize) { return mlc::String("", 0); },
[&](const registry::TChar& tChar) { return mlc::String("", 0); },
[&](const registry::TUnknown& tUnknown) { return mlc::String("", 0); },
[&](const registry::TArray& tArray) { auto [inner] = tArray; return mlc::String("", 0); },
[&](const registry::TShared& tShared) { auto [inner] = tShared; return mlc::String("", 0); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return mlc::String("", 0); },
[&](const registry::TPair& tPair) { auto [left, right] = tPair; return mlc::String("", 0); },
[&](const registry::TTuple& tTuple) { auto [types] = tTuple; return mlc::String("", 0); },
[&](const registry::TFn& tFn) { auto [param_types, return_type] = tFn; return mlc::String("", 0); },
[&](const registry::TAssoc& tAssoc) { auto [param, assoc] = tAssoc; return mlc::String("", 0); }
}, (*constructor_type));
}
int callee_min_arity(registry::TypeRegistry registry, mlc::String callee_name, int max_arity) noexcept{
  if (((callee_name != mlc::String("", 0)) && registry::TypeRegistry_has_fn(registry, callee_name)))   {
    auto required_arity = registry::TypeRegistry_required_arity_for_fn(registry, callee_name);
    if ((required_arity >= 0))     {
      return required_arity;
    } else     {
      return max_arity;
    }
  } else   {
    return max_arity;
  }
}
std::shared_ptr<registry::Type> return_type_from_inferred_function_type(std::shared_ptr<registry::Type> inferred_type) noexcept{
  if (semantic_type_structure::type_is_function(inferred_type))   {
    return semantic_type_structure::function_return_type(inferred_type);
  } else   {
    return std::make_shared<registry::Type>(registry::TUnknown{});
  }
}
infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
  auto constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
  auto call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.length(), call_arguments.length(), call_source_span);
  auto constructor_type = registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name);
  auto owner_name = owner_name_from_constructor_type(constructor_type);
  auto private_errors = ((registry::TypeRegistry_is_private_ctor(inference_context.registry, constructor_name) && (inference_context.current_extend_type != owner_name)) ? (mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("private constructor: cannot construct ", 38) + constructor_name) + mlc::String(" outside its extend block", 25)), call_source_span, diagnostic_codes::diagnostic_code_e065())}) : (mlc::Array<ast::Diagnostic>{}));
  return infer_result::InferResult{constructor_type, ast::diagnostics_append(ast::diagnostics_append(with_arguments.errors, call_errors), private_errors)};
}
infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_parsed, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
  auto parameter_types = semantic_type_structure::function_parameter_list(function_parsed.inferred_type);
  auto max_arity = parameter_types.length();
  auto min_arity = callee_min_arity(registry, callee_name, max_arity);
  auto arity_errors = ((min_arity == max_arity) ? (type_diagnostics::call_arity_diagnostics(max_arity, call_arguments.length(), call_source_span)) : (type_diagnostics::call_arity_range_diagnostics(min_arity, max_arity, call_arguments.length(), call_source_span)));
  return infer_result::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}
infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_parsed, ast::Span call_source_span) noexcept{
  if (semantic_type_structure::type_is_unknown(function_parsed.inferred_type))   {
    return base;
  } else   {
    return infer_result::InferResult{std::make_shared<registry::Type>(registry::TUnknown{}), ast::diagnostics_append(base.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(((mlc::String("called value is not a function (got ", 36) + semantic_type_structure::type_description(function_parsed.inferred_type)) + mlc::String(")", 1)), call_source_span, diagnostic_codes::diagnostic_code_e002())})};
  }
}
infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
  auto return_type = return_type_from_inferred_function_type(function_parsed.inferred_type);
  auto base = infer_result::InferResult{return_type, with_arguments.errors};
  if (semantic_type_structure::type_is_function(function_parsed.inferred_type))   {
    return infer_expr_call_non_constructor_arity_only(base, function_parsed, call_arguments, call_source_span, return_type, registry, callee_name);
  } else   {
    return infer_expr_call_non_constructor_not_callable(base, function_parsed, call_source_span);
  }
}

} // namespace infer_call_support
