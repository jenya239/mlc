#include "check_context.hpp"

#include "registry.hpp"

namespace check_context {

using namespace registry;

check_context::CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;

check_context::CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{return check_context::CheckContext{type_environment, registry};}

} // namespace check_context
