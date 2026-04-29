#include "infer_call_support.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "registry.hpp"
#include "infer_result.hpp"
#include "type_diagnostics.hpp"
#include "semantic_type_structure.hpp"

namespace infer_call_support {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace registry;
using namespace infer_result;
using namespace type_diagnostics;
using namespace semantic_type_structure;
using namespace ast_tokens;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept;

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept;

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept;

infer_result::InferResult infer_expr_call_for_constructor_name(mlc::String constructor_name, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, check_context::CheckContext inference_context) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> constructor_parameter_types = registry::TypeRegistry_ctor_params_for(inference_context.registry, constructor_name);
mlc::Array<ast::Diagnostic> call_errors = type_diagnostics::constructor_arity_diagnostics(constructor_parameter_types.size(), call_arguments.size(), call_source_span);
std::shared_ptr<registry::Type> ctor_type = registry::TypeRegistry_ctor_type(inference_context.registry, constructor_name);
mlc::String owner_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*ctor_type))) { auto _v_tnamed = std::get<registry::TNamed>((*ctor_type)); auto [n] = _v_tnamed; return n; } return mlc::String(""); }();
mlc::Array<ast::Diagnostic> private_errors = registry::TypeRegistry_is_private_ctor(inference_context.registry, constructor_name) && inference_context.current_extend_type != owner_name ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("private constructor: cannot construct ") + constructor_name + mlc::String(" outside its extend block"), call_source_span)} : mlc::Array<ast::Diagnostic>{};
return infer_result::InferResult{ctor_type, ast::diagnostics_append(ast::diagnostics_append(with_arguments.errors, call_errors), private_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_arity_only(infer_result::InferResult base, infer_result::InferResult function_result, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, std::shared_ptr<registry::Type> return_type, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
mlc::Array<std::shared_ptr<registry::Type>> parameter_types = semantic_type_structure::function_parameter_list(function_result.inferred_type);
int max_arity = parameter_types.size();
int min_arity = callee_name != mlc::String("") && registry::TypeRegistry_has_fn(registry, callee_name) ? [&]() -> int { 
  int r = registry::TypeRegistry_required_arity_for_fn(registry, callee_name);
  return r >= 0 ? r : max_arity;
 }() : max_arity;
mlc::Array<ast::Diagnostic> arity_errors = min_arity == max_arity ? type_diagnostics::call_arity_diagnostics(max_arity, call_arguments.size(), call_source_span) : type_diagnostics::call_arity_range_diagnostics(min_arity, max_arity, call_arguments.size(), call_source_span);
return infer_result::InferResult{return_type, ast::diagnostics_append(base.errors, arity_errors)};
}

infer_result::InferResult infer_expr_call_non_constructor_not_callable(infer_result::InferResult base, infer_result::InferResult function_result, ast::Span call_source_span) noexcept{return semantic_type_structure::type_is_unknown(function_result.inferred_type) ? base : infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(base.errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error(mlc::String("called value is not a function (got ") + semantic_type_structure::type_description(function_result.inferred_type) + mlc::String(")"), call_source_span)})};}

infer_result::InferResult infer_expr_call_non_constructor(infer_result::InferResult function_result, infer_result::InferResult with_arguments, mlc::Array<std::shared_ptr<ast::Expr>> call_arguments, ast::Span call_source_span, registry::TypeRegistry registry, mlc::String callee_name) noexcept{
std::shared_ptr<registry::Type> return_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TFn>((*function_result.inferred_type))) { auto _v_tfn = std::get<registry::TFn>((*function_result.inferred_type)); auto [_w0, return_ty] = _v_tfn; return return_ty; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();
infer_result::InferResult base = infer_result::InferResult{return_type, with_arguments.errors};
return semantic_type_structure::type_is_function(function_result.inferred_type) ? infer_expr_call_non_constructor_arity_only(base, function_result, call_arguments, call_source_span, return_type, registry, callee_name) : infer_expr_call_non_constructor_not_callable(base, function_result, call_source_span);
}

} // namespace infer_call_support
