#include "check_context.hpp"

#include "registry.hpp"

namespace check_context {

using namespace registry;

check_context::CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;

check_context::CheckContext check_context_with_expected_return(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_return_type) noexcept;

check_context::CheckContext check_context_child(check_context::CheckContext parent, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment) noexcept;

check_context::CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept{return check_context::CheckContext{type_environment, registry, mlc::String(""), std::make_shared<registry::Type>((registry::TUnknown{}))};}

check_context::CheckContext check_context_with_expected_return(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_return_type) noexcept{return check_context::CheckContext{type_environment, registry, mlc::String(""), expected_return_type};}

check_context::CheckContext check_context_child(check_context::CheckContext parent, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment) noexcept{return check_context::CheckContext{type_environment, parent.registry, parent.current_extend_type, parent.expected_return_type};}

} // namespace check_context
