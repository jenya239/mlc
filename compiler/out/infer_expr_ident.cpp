#define main mlc_user_main
#include "infer_expr_ident.hpp"

#include "check_context.hpp"
#include "infer_result.hpp"
#include "registry.hpp"

namespace infer_expr_ident {

using namespace check_context;
using namespace infer_result;
using namespace registry;

infer_result::InferResult infer_expr_identifier(mlc::String name, check_context::CheckContext inference_context) noexcept{
  if (inference_context.type_env.has(name))   {
    return infer_result::infer_ok(inference_context.type_env.get(name));
  } else if (registry::TypeRegistry_has_fn(inference_context.registry, name))   {
    return infer_result::infer_ok(registry::TypeRegistry_fn_type(inference_context.registry, name));
  } else if (registry::TypeRegistry_has_ctor(inference_context.registry, name))   {
    return infer_result::infer_ok(registry::TypeRegistry_ctor_type(inference_context.registry, name));
  } else if (inference_context.registry.adt_index.algebraic_decl_type_parameter_names.has(name))   {
    return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TNamed{name}));
  } else   {
    return infer_result::infer_ok(std::make_shared<registry::Type>(registry::TUnknown{}));
  }
}

} // namespace infer_expr_ident
