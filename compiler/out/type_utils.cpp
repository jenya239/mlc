#include "type_utils.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace type_utils {

using namespace registry;
using namespace semantic_type_structure;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept;

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept;

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept;

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept;

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept;

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept;

int builtin_method_expected_argument_count(mlc::String method_name) noexcept;

mlc::String type_description(std::shared_ptr<registry::Type> type_value) noexcept{return std::visit(overloaded{
  [&](const TI32& ti32) -> mlc::String { return mlc::String("i32"); },
  [&](const TString& tstring) -> mlc::String { return mlc::String("string"); },
  [&](const TBool& tbool) -> mlc::String { return mlc::String("bool"); },
  [&](const TUnit& tunit) -> mlc::String { return mlc::String("unit"); },
  [&](const TUnknown& tunknown) -> mlc::String { return mlc::String("unknown"); },
  [&](const TArray& tarray) -> mlc::String { auto [_w0] = tarray; return mlc::String("array"); },
  [&](const TShared& tshared) -> mlc::String { auto [_w0] = tshared; return mlc::String("shared"); },
  [&](const TNamed& tnamed) -> mlc::String { auto [type_name] = tnamed; return type_name; },
  [&](const TGeneric& tgeneric) -> mlc::String { auto [type_name, _w0] = tgeneric; return type_name; },
  [&](const TFn& tfn) -> mlc::String { auto [_w0, _w1] = tfn; return mlc::String("function"); }
}, (*type_value));}

bool type_is_i32(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TI32>((*type_value))) {  return true; } return false; }();}

bool type_is_bool(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TBool>((*type_value))) {  return true; } return false; }();}

bool type_is_string(std::shared_ptr<registry::Type> type_value) noexcept{return [&]() { if (std::holds_alternative<registry::TString>((*type_value))) {  return true; } return false; }();}

bool types_allowed_for_binary_plus(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return semantic_type_structure::type_is_unknown(left_type) || semantic_type_structure::type_is_unknown(right_type) || type_is_i32(left_type) && type_is_i32(right_type) || type_is_string(left_type) && type_is_string(right_type);}

bool types_allowed_for_binary_int_only(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return semantic_type_structure::type_is_unknown(left_type) || semantic_type_structure::type_is_unknown(right_type) || type_is_i32(left_type) && type_is_i32(right_type);}

bool types_allowed_for_ordering(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return semantic_type_structure::type_is_unknown(left_type) || semantic_type_structure::type_is_unknown(right_type) || type_is_i32(left_type) && type_is_i32(right_type) || type_is_string(left_type) && type_is_string(right_type);}

bool types_allowed_for_logical_and_or(std::shared_ptr<registry::Type> left_type, std::shared_ptr<registry::Type> right_type) noexcept{return semantic_type_structure::type_is_unknown(left_type) || semantic_type_structure::type_is_unknown(right_type) || type_is_bool(left_type) && type_is_bool(right_type);}

bool receiver_type_is_map(std::shared_ptr<registry::Type> receiver_type) noexcept{return [&]() { if (std::holds_alternative<registry::TNamed>((*receiver_type))) { auto _v_tnamed = std::get<registry::TNamed>((*receiver_type)); auto [name] = _v_tnamed; return name == mlc::String("Map"); } if (std::holds_alternative<registry::TGeneric>((*receiver_type))) { auto _v_tgeneric = std::get<registry::TGeneric>((*receiver_type)); auto [name, _w0] = _v_tgeneric; return name == mlc::String("Map"); } return false; }();}

std::shared_ptr<registry::Type> binary_operation_result_type(mlc::String operation, std::shared_ptr<registry::Type> left_type) noexcept{
return operation == mlc::String("+") || operation == mlc::String("-") || operation == mlc::String("*") || operation == mlc::String("/") || operation == mlc::String("%") ? [&]() { if (std::holds_alternative<registry::TString>((*left_type))) {  return std::make_shared<registry::Type>((registry::TString{})); } return std::make_shared<registry::Type>((registry::TI32{})); }() : operation == mlc::String("=") ? std::make_shared<registry::Type>((registry::TUnit{})) : std::make_shared<registry::Type>((registry::TBool{}));
}

std::shared_ptr<registry::Type> builtin_method_return_type(mlc::String method_name) noexcept{
return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("push") || method_name == mlc::String("set") || method_name == mlc::String("remove") ? std::make_shared<registry::Type>((registry::TUnit{})) : method_name == mlc::String("keys") || method_name == mlc::String("values") ? std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>((registry::TUnknown{})))) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<registry::Type>((registry::TString{})) : method_name == mlc::String("has") ? std::make_shared<registry::Type>((registry::TBool{})) : std::make_shared<registry::Type>((registry::TUnknown{}));
}

std::shared_ptr<registry::Type> field_type_from_object(std::shared_ptr<registry::Type> object_type, mlc::String field_name, registry::TypeRegistry registry) noexcept{
std::shared_ptr<registry::Type> inner_type = [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TShared>((*object_type))) { auto _v_tshared = std::get<registry::TShared>((*object_type)); auto [inner] = _v_tshared; return inner; } return object_type; }();
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<registry::TNamed>((*inner_type))) { auto _v_tnamed = std::get<registry::TNamed>((*inner_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && registry::TypeRegistry_has_fields(registry, type_name) ? [&]() -> std::shared_ptr<registry::Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

int builtin_method_expected_argument_count(mlc::String method_name) noexcept{
return method_name == mlc::String("push") ? 1 : method_name == mlc::String("set") ? 2 : method_name == mlc::String("length") || method_name == mlc::String("size") ? 0 : method_name == mlc::String("to_i") || method_name == mlc::String("to_lower") ? 0 : method_name == mlc::String("char_at") ? 1 : method_name == mlc::String("substring") ? 2 : method_name == mlc::String("join") ? 1 : method_name == mlc::String("to_string") ? 0 : method_name == mlc::String("has") || method_name == mlc::String("get") || method_name == mlc::String("remove") ? 1 : method_name == mlc::String("keys") || method_name == mlc::String("values") ? 0 : -1;
}

} // namespace type_utils
