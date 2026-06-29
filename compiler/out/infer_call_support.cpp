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

mlc::String owner_name_from_constructor_type(std::shared_ptr<registry::Type> constructor_type) noexcept;

int callee_min_arity(registry::TypeRegistry registry, mlc::String callee_name, int max_arity) noexcept;

std::shared_ptr<registry::Type> return_type_from_inferred_function_type(std::shared_ptr<registry::Type> inferred_type) noexcept;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_parsed, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_parsed, ast::Span call_source_span) noexcept;

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept;

mlc::String owner_name_from_constructor_type(std::shared_ptr<registry::Type> constructor_type) noexcept{return std::visit(overloaded{
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return type_name; },
  [&](const TI32& ti32) -> mlc::String { return mlc::String(""); },
  [&](const TString& tstring) -> mlc::String { return mlc::String(""); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String(""); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String(""); },
  [&](const TI64& ti64) -> mlc::String { return mlc::String(""); },
  [&](const TF64& tf64) -> mlc::String { return mlc::String(""); },
  [&](const TU8& tu8) -> mlc::String { return mlc::String(""); },
  [&](const TUsize& tusize) -> mlc::String { return mlc::String(""); },
  [&](const TChar& tchar) -> mlc::String { return mlc::String(""); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String(""); },
  [&](const TArray& tarray) -> mlc::String { auto [inner] = tarray; return mlc::String(""); },
  [&](const TShared& tshared) -> mlc::String { auto [inner] = tshared; return mlc::String(""); },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, type_arguments] = tgeneric; return mlc::String(""); },
  [&](const TPair& tpair) -> mlc::String { auto [left, right] = tpair; return mlc::String(""); },
  [&](const TTuple& ttuple) -> mlc::String { auto [types] = ttuple; return mlc::String(""); },
  [&](const TFn& tfn) -> mlc::String { auto [param_types, return_type] = tfn; return mlc::String(""); },
  [&](const TAssoc& tassoc) -> mlc::String { auto [param, assoc] = tassoc; return mlc::String(""); }
}, (*constructor_type));}

int callee_min_arity(registry::TypeRegistry registry, mlc::String callee_name, int max_arity) noexcept{return callee_name != mlc::String("") && registry::TypeRegistry_has_fn(registry, callee_name) ? [&]() -> int { 
  int required_arity = registry::TypeRegistry_required_arity_for_fn(registry, callee_name);
  return required_arity >= 0 ? required_arity : max_arity;
 }() : max_arity;}

std::shared_ptr<registry::Type> return_type_from_inferred_function_type(std::shared_ptr<registry::Type> inferred_type) noexcept{return semantic_type_structure::type_is_function(inferred_type) ? semantic_type_structure::function_return_type(inferred_type) : std::make_shared<registry::Type>((registry::TUnknown{}));}

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
mlc::Array<ast::Diagnostic> call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.size(), call_arguments.size(), call_source_span);
std::shared_ptr<registry::Type> constructor_type = registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name);
mlc::String owner_name = owner_name_from_constructor_type(constructor_type);
mlc::Array<ast::Diagnostic> private_errors = registry::TypeRegistry_is_private_ctor(inference_context.registry, constructor_name) && inference_context.current_extend_type != owner_name ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("private constructor: cannot construct ") + constructor_name + mlc::String(" outside its extend block"), call_source_span, diagnostic_codes::diagnostic_code_e065())} : mlc::Array<ast::Diagnostic>{};
return infer_result::InferResult{constructor_type, ast::diagnostics_append(ast::diagnostics_append(with_arguments.errors, call_errors), private_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_parsed, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> parameter_types = semantic_type_structure::function_parameter_list(function_parsed.inferred_type);
int max_arity = parameter_types.size();
int min_arity = callee_min_arity(registry, callee_name, max_arity);
mlc::Array<ast::Diagnostic> arity_errors = min_arity == max_arity ? type_diagnostics::call_arity_diagnostics(max_arity, call_arguments.size(), call_source_span) : type_diagnostics::call_arity_range_diagnostics(min_arity, max_arity, call_arguments.size(), call_source_span);
return infer_result::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_parsed, ast::Span call_source_span) noexcept{return semantic_type_structure::type_is_unknown(function_parsed.inferred_type) ? base : infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(base.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("called value is not a function (got ") + semantic_type_structure::type_description(function_parsed.inferred_type) + mlc::String(")"), call_source_span, diagnostic_codes::diagnostic_code_e002())})};}

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_parsed, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
std::shared_ptr<registry::Type> return_type = return_type_from_inferred_function_type(function_parsed.inferred_type);
infer_result::InferResult base = infer_result::InferResult{return_type, with_arguments.errors};
return semantic_type_structure::type_is_function(function_parsed.inferred_type) ? infer_expr_call_non_constructor_arity_only(base, function_parsed, call_arguments, call_source_span, return_type, registry, callee_name) : infer_expr_call_non_constructor_not_callable(base, function_parsed, call_source_span);
}

} // namespace infer_call_support
