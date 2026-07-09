#include "infer_weak_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "type_diagnostics.hpp"

namespace infer_weak_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace type_diagnostics;
using namespace ast_tokens;

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> weak_pointer_inner_type(std::shared_ptr<registry::Type> type_value) noexcept;

bool is_shared_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;

infer_result::InferResult infer_shared_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool is_shared_weak_method(mlc::String method_name) noexcept;

bool is_weak_upgrade_method(mlc::String method_name) noexcept;

bool is_shared_weak_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

bool is_weak_upgrade_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept;

infer_result::InferResult infer_shared_weak_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept;

infer_result::InferResult infer_weak_upgrade_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept;

bool type_is_weak_pointer(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [name, type_arguments] = _v_tgeneric; return name == mlc::String("Weak") && type_arguments.size() == 1; } return false; }();}

std::shared_ptr<registry::Type> weak_pointer_inner_type(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [_w0, type_arguments] = _v_tgeneric; return type_arguments.size() == 1 ? type_arguments[0] : std::make_shared<registry::Type>((registry::TUnknown{})); } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool is_shared_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return name == mlc::String("Shared") && method_name == mlc::String("new"); } return false; }();}

infer_result::InferResult infer_shared_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::Array<ast::Diagnostic> empty_errors = {};
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, mlc::String("new"), method_arguments.size(), method_span);
return method_arguments.size() != 1 ? infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(object_parsed.errors, arity_errors)} : [&]() -> infer_result::InferResult { 
  infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TShared(argument_parsed.inferred_type)), ast::diagnostics_append(ast::diagnostics_append(object_parsed.errors, arity_errors), argument_parsed.errors)};
 }();
}

bool is_shared_weak_method(mlc::String method_name) noexcept{return method_name == mlc::String("weak") || method_name == mlc::String("downgrade");}

bool is_weak_upgrade_method(mlc::String method_name) noexcept{return method_name == mlc::String("upgrade") || method_name == mlc::String("lock");}

bool is_shared_weak_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return semantic_type_structure::type_is_shared_pointer(receiver_type) && is_shared_weak_method(method_name);}

bool is_weak_upgrade_method_on_receiver(std::shared_ptr<registry::Type> receiver_type, mlc::String method_name) noexcept{return type_is_weak_pointer(receiver_type) && is_weak_upgrade_method(method_name);}

infer_result::InferResult infer_shared_weak_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept{
mlc::Array<ast::Diagnostic> empty_errors = {};
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, method_name, method_arguments.size(), method_span);
return method_arguments.size() != 0 ? infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(object_parsed.errors, arity_errors)} : [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> inner_type = semantic_type_structure::shared_pointer_inner_type(object_parsed.inferred_type);
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Weak"), mlc::Array<std::shared_ptr<registry::Type>>{inner_type})), ast::diagnostics_append(object_parsed.errors, arity_errors)};
 }();
}

infer_result::InferResult infer_weak_upgrade_method_call(infer_result::InferResult object_parsed, mlc::String method_name, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext _inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> _infer_expr_fn) noexcept{
mlc::Array<ast::Diagnostic> empty_errors = {};
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(empty_errors, method_name, method_arguments.size(), method_span);
return method_arguments.size() != 0 ? infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(object_parsed.errors, arity_errors)} : [&]() -> infer_result::InferResult { 
  std::shared_ptr<registry::Type> inner_type = weak_pointer_inner_type(object_parsed.inferred_type);
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{std::make_shared<registry::Type>(registry::TShared(inner_type))})), ast::diagnostics_append(object_parsed.errors, arity_errors)};
 }();
}

} // namespace infer_weak_method
