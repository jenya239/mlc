#ifndef CHECK_CONTEXT_HPP
#define CHECK_CONTEXT_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
namespace check_context {

struct CheckContext {
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_env;
  registry::TypeRegistry registry;
  mlc::String current_extend_type;
  std::shared_ptr<registry::Type> expected_return_type;
};
CheckContext check_context_new(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry) noexcept;
CheckContext check_context_with_expected_return(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment, registry::TypeRegistry registry, std::shared_ptr<registry::Type> expected_return_type) noexcept;
CheckContext check_context_child(CheckContext parent, mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> type_environment) noexcept;

} // namespace check_context

#endif // CHECK_CONTEXT_HPP
