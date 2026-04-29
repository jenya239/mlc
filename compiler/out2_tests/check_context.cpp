#define main mlc_user_main
#include "check_context.hpp"

namespace check_context {

CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{
return CheckContext{type_environment, registry};
}

} // namespace check_context
