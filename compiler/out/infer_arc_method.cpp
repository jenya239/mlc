#include "infer_arc_method.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"
#include "semantic_type_structure.hpp"
#include "send_safe.hpp"
#include "type_diagnostics.hpp"
#include "diagnostic_codes.hpp"

namespace infer_arc_method {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace semantic_type_structure;
using namespace send_safe;
using namespace type_diagnostics;
using namespace diagnostic_codes;
using namespace ast_tokens;

bool is_arc_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept;

infer_result::InferResult infer_arc_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

bool is_arc_static_new(std::shared_ptr<ast::Expr> object, mlc::String method_name) noexcept{return [&]() { if (std::holds_alternative<ast::ExprIdent>((*object)._)) { auto _v_exprident = std::get<ast::ExprIdent>((*object)._); auto [name, _w0] = _v_exprident; return name == mlc::String("Arc") && method_name == mlc::String("new"); } return false; }();}

infer_result::InferResult infer_arc_static_new_call(infer_result::InferResult object_parsed, mlc::Array<std::shared_ptr<ast::Expr>> method_arguments, ast::Span method_span, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::Array<ast::Diagnostic> receiver_errors = {};
mlc::Array<ast::Diagnostic> arity_errors = type_diagnostics::method_arity_after_receiver(receiver_errors, mlc::String("new"), method_arguments.size(), method_span);
return method_arguments.size() != 1 ? infer_result::InferResult{std::make_shared<registry::Type>((registry::TUnknown{})), ast::diagnostics_append(object_parsed.errors, arity_errors)} : [&]() -> infer_result::InferResult { 
  infer_result::InferResult argument_parsed = infer_expr_fn(method_arguments[0], inference_context);
  std::shared_ptr<registry::Type> argument_type = argument_parsed.inferred_type;
  mlc::Array<ast::Diagnostic> errors = ast::diagnostics_append(object_parsed.errors, arity_errors);
  errors = ast::diagnostics_append(errors, argument_parsed.errors);
  if (!semantic_type_structure::type_is_unknown(argument_type) && !send_safe::type_is_send_safe(argument_type, inference_context.registry)){
{
errors = ast::diagnostics_append(errors, mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("Arc.new requires a Send-safe value, got ") + semantic_type_structure::type_description(argument_type), ast::expr_span(method_arguments[0]), diagnostic_codes::diagnostic_code_e082())});
}
}
  return infer_result::InferResult{std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Arc"), mlc::Array<std::shared_ptr<registry::Type>>{argument_type})), errors};
 }();
}

} // namespace infer_arc_method
