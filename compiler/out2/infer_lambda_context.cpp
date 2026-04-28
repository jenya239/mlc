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

using InferExprFn = std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)>;

infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept;

infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, std::function<infer_result::InferResult(std::shared_ptr<ast::Expr>, check_context::CheckContext)> infer_expr_fn) noexcept{
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> lambda_env = inference_context.type_env;
mlc::Array<std::shared_ptr<registry::Type>> built = {};
int index = 0;
while (index < parameter_names.size()){
{
std::shared_ptr<registry::Type> param_type = index < parameter_types.size() ? parameter_types[index] : std::make_shared<registry::Type>((registry::TUnknown{}));
built.push_back(param_type);
lambda_env.set(parameter_names[index], param_type);
index = index + 1;
}
}
check_context::CheckContext inner_context = check_context::check_context_new(lambda_env, inference_context.registry);
infer_result::InferResult body_result = infer_expr_fn(body, inner_context);
return infer_result::InferResult{std::make_shared<registry::Type>(registry::TFn(built, body_result.inferred_type)), body_result.errors};
}

} // namespace infer_lambda_context
