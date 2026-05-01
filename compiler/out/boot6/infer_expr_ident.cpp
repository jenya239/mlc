#define main mlc_user_main
#include "infer_expr_ident.hpp"

namespace infer_expr_ident {

infer_result::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept{
if (inference_context.type_env.has(name)) {
return infer_result::infer_ok(inference_context.type_env.get(name));
} else if (registry::TypeRegistry_has_fn(inference_context.registry, name)) {
return infer_result::infer_ok(registry::TypeRegistry_fn_type(inference_context.registry, name));
} else if (registry::TypeRegistry_has_ctor(inference_context.registry, name)) {
return infer_result::infer_ok(registry::TypeRegistry_ctor_type(inference_context.registry, name));
} else {
return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{}));
}


}

} // namespace infer_expr_ident
