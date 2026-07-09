#ifndef SEND_SAFE_HPP
#define SEND_SAFE_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
namespace send_safe {

bool type_is_send_safe(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;
bool type_is_send_safe_field(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map, mlc::String field_name, registry::TypeRegistry registry) noexcept;
bool type_is_send_safe_named(mlc::String record_name, registry::TypeRegistry registry) noexcept;
bool type_is_send_safe_generic(mlc::String type_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept;

} // namespace send_safe

#endif // SEND_SAFE_HPP
