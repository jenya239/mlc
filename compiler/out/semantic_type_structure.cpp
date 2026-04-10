#include "semantic_type_structure.hpp"

#include "registry.hpp"

namespace semantic_type_structure {

using namespace registry;

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept;

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept;

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept;

bool type_is_array(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [_w0] = _v_tarray; return true; } return false; }();}

std::shared_ptr<registry::Type> array_element_type_from_array_type(std::shared_ptr<registry::Type> array_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*array_type))) { auto _v_tarray = std::get<registry::TArray>((*array_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

bool type_is_shared_pointer(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); auto [_w0] = _v_tshared; return true; } return false; }();}

std::shared_ptr<registry::Type> shared_pointer_inner_type(std::shared_ptr<registry::Type> shared_pointer_type) noexcept{return [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*shared_pointer_type))) { auto _v_tshared = std::get<registry::TShared>((*shared_pointer_type)); auto [inner] = _v_tshared; return inner; } return std::make_shared<registry::Type>((registry::TUnknown{})); }();}

} // namespace semantic_type_structure
