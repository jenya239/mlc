#define main mlc_user_main
#include "infer_lambda_context.hpp"

namespace infer_lambda_context {

void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
void __skip__() noexcept{
return /* unit */;
}
infer_result::InferResult infer_lambda_with_param_types(mlc::Array<mlc::String> parameter_names, mlc::Array<std::shared_ptr<registry::Type>> parameter_types, std::shared_ptr<ast::Expr> body, check_context::CheckContext inference_context, InferExprFn infer_expr_fn) noexcept{
auto lambda_env = inference_context.type_env;
auto built = mlc::Array<std::shared_ptr<registry::Type>>{};
auto index = 0;
return [&]() {
while ((index < parameter_names.length())) {
auto param_type = ((index < parameter_types.length()) ? parameter_types[index] : [&]() {
std::make_shared<registry::Type>(registry::TUnknown{});
built.push_back(param_type);
lambda_env.set(parameter_names[index], param_type);
index = (index + 1);
return /* unit */;
}());
auto inner_context = check_context::check_context_new(lambda_env, inference_context.registry);
auto body_result = infer_expr_fn(body, inner_context);
infer_result::InferResult{std::make_shared<registry::Type>(registry::TFn{built, body_result.inferred_type}), body_result.errors};
}
}();
}

} // namespace infer_lambda_context
