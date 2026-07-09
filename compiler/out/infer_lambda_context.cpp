#include "infer_lambda_context.hpp"

#include "ast.hpp"
#include "registry.hpp"
#include "check_context.hpp"
#include "infer_result.hpp"

namespace infer_lambda_context {

using namespace ast;
using namespace registry;
using namespace check_context;
using namespace infer_result;
using namespace ast_tokens;

infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_environment = inference_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> built_parameter_types = parameter_names.fold([&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  mlc::Array<std::shared_ptr<registry::Type>> empty_parameter_types = {};
  return empty_parameter_types;
 }(), [parameter_types, &lambda_environment](mlc::Array<std::shared_ptr<registry::Type>> built_parameter_types_so_far, mlc::String parameter_name) mutable { return [&]() -> mlc::Array<std::shared_ptr<registry::Type>> { 
  int parameter_index = built_parameter_types_so_far.size();
  std::shared_ptr<registry::Type> parameter_type = parameter_index < parameter_types.size() ? parameter_types[parameter_index] : std::make_shared<registry::Type>((registry::TUnknown{}));
  lambda_environment.set(parameter_name, parameter_type);
  return built_parameter_types_so_far.concat(mlc::Array<std::shared_ptr<registry::Type>>{parameter_type});
 }(); });
check_context::CheckContext lambda_body_context = check_context::check_context_child(inference_context, lambda_environment);
infer_result::InferResult body_parsed = infer_expr_fn(body, lambda_body_context);
return infer_result::InferResult{std::make_shared<registry::Type>(registry::TFn(built_parameter_types, body_parsed.inferred_type)), body_parsed.errors};
}

} // namespace infer_lambda_context
