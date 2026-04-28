#include "array_method_types.hpp"

#include "registry.hpp"
#include "semantic_type_structure.hpp"

namespace array_method_types {

using namespace registry;
using namespace semantic_type_structure;

bool is_array_hof_method(mlc::String method_name) noexcept;

int array_hof_expected_argument_count(mlc::String method_name) noexcept;

std::shared_ptr<registry::Type> array_hof_call_result_type(std::shared_ptr<registry::Type> element_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept;

bool is_array_hof_method(mlc::String method_name) noexcept{return method_name == mlc::String("map") || method_name == mlc::String("filter") || method_name == mlc::String("fold") || method_name == mlc::String("flat_map") || method_name == mlc::String("any") || method_name == mlc::String("all") || method_name == mlc::String("find_index") || method_name == mlc::String("find") || method_name == mlc::String("sort_by") || method_name == mlc::String("take") || method_name == mlc::String("drop") || method_name == mlc::String("zip") || method_name == mlc::String("enumerate") || method_name == mlc::String("join") || method_name == mlc::String("sum") || method_name == mlc::String("group_by") || method_name == mlc::String("flat");}

int array_hof_expected_argument_count(mlc::String method_name) noexcept{return method_name == mlc::String("fold") ? 2 : method_name == mlc::String("enumerate") || method_name == mlc::String("sum") || method_name == mlc::String("flat") ? 0 : 1;}

std::shared_ptr<registry::Type> array_hof_call_result_type(std::shared_ptr<registry::Type> element_type, mlc::String method_name, mlc::Array<std::shared_ptr<registry::Type>> argument_inferred_types) noexcept{
return method_name == mlc::String("map") ? argument_inferred_types.size() > 0 ? std::make_shared<registry::Type>(registry::TArray(semantic_type_structure::function_return_type(argument_inferred_types[0]))) : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("filter") || method_name == mlc::String("sort_by") ? std::make_shared<registry::Type>(registry::TArray(element_type)) : method_name == mlc::String("fold") ? argument_inferred_types.size() > 0 ? argument_inferred_types[0] : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("flat_map") ? argument_inferred_types.size() > 0 ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> callback_return = semantic_type_structure::function_return_type(argument_inferred_types[0]);
  return semantic_type_structure::type_is_array(callback_return) ? [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*callback_return))) { auto _v_tarray = std::get<registry::TArray>((*callback_return)); auto [inner] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(inner)); } return std::make_shared<registry::Type>((registry::TUnknown{})); }() : std::make_shared<registry::Type>((registry::TUnknown{}));
 }() : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("any") || method_name == mlc::String("all") ? std::make_shared<registry::Type>((registry::TBool{})) : method_name == mlc::String("find_index") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("find") ? std::make_shared<registry::Type>(registry::TGeneric(mlc::String("Option"), mlc::Array<std::shared_ptr<registry::Type>>{element_type})) : method_name == mlc::String("take") || method_name == mlc::String("drop") ? std::make_shared<registry::Type>(registry::TArray(element_type)) : method_name == mlc::String("zip") ? argument_inferred_types.size() > 0 ? [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*argument_inferred_types[0]))) { auto _v_tarray = std::get<registry::TArray>((*argument_inferred_types[0])); auto [other_el] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>(registry::TPair(element_type, other_el)))); } return std::make_shared<registry::Type>((registry::TUnknown{})); }() : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("enumerate") ? std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>(registry::TPair(std::make_shared<registry::Type>((registry::TI32{})), element_type)))) : method_name == mlc::String("join") ? std::make_shared<registry::Type>((registry::TString{})) : method_name == mlc::String("sum") ? std::make_shared<registry::Type>((registry::TI32{})) : method_name == mlc::String("group_by") ? argument_inferred_types.size() > 0 ? [&]() -> std::shared_ptr<registry::Type> { 
  std::shared_ptr<registry::Type> key_t = semantic_type_structure::function_return_type(argument_inferred_types[0]);
  return std::make_shared<registry::Type>(registry::TArray(std::make_shared<registry::Type>(registry::TPair(key_t, std::make_shared<registry::Type>(registry::TArray(element_type))))));
 }() : std::make_shared<registry::Type>((registry::TUnknown{})) : method_name == mlc::String("flat") ? [&]() -> std::shared_ptr<registry::Type> { if (std::holds_alternative<registry::TArray>((*element_type))) { auto _v_tarray = std::get<registry::TArray>((*element_type)); auto [inner] = _v_tarray; return std::make_shared<registry::Type>(registry::TArray(inner)); } return std::make_shared<registry::Type>((registry::TUnknown{})); }() : std::make_shared<registry::Type>((registry::TUnknown{}));
}

} // namespace array_method_types
