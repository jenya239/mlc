#define main mlc_user_main
#include "check_context.hpp"

#include "registry.hpp"

namespace check_context {

using namespace registry;

CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{
  return CheckContext{type_environment, registry, mlc::String("", 0), std::make_shared<registry::Type>(registry::TUnknown{})};
}
CheckContext check_context_with_expected_return(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_return_type) noexcept{
  return CheckContext{type_environment, registry, mlc::String("", 0), expected_return_type};
}
CheckContext check_context_child(CheckContext parent, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment) noexcept{
  return CheckContext{type_environment, parent.registry, parent.current_extend_type, parent.expected_return_type};
}

} // namespace check_context
