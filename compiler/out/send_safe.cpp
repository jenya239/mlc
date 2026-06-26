#include "send_safe.hpp"

#include "registry.hpp"

namespace send_safe {

using namespace registry;

bool type_is_send_safe(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept;

bool type_is_send_safe_field(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map, mlc::String field_name, registry::TypeRegistry registry) noexcept;

bool type_is_send_safe_named(mlc::String record_name, registry::TypeRegistry registry) noexcept;

bool type_is_send_safe_generic(mlc::String type_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept;

bool type_is_send_safe(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{return [&]() { if (std::holds_alternative<registry::TUnknown>((*type_value))) {  return true; } if (std::holds_alternative<registry::TI32>((*type_value)) || std::holds_alternative<registry::TString>((*type_value)) || std::holds_alternative<registry::TBool>((*type_value)) || std::holds_alternative<registry::TUnit>((*type_value)) || std::holds_alternative<registry::TI64>((*type_value)) || std::holds_alternative<registry::TF64>((*type_value)) || std::holds_alternative<registry::TU8>((*type_value)) || std::holds_alternative<registry::TUsize>((*type_value)) || std::holds_alternative<registry::TChar>((*type_value))) { return true; } if (std::holds_alternative<registry::TArray>((*type_value))) { auto _v_tarray = std::get<registry::TArray>((*type_value)); auto [_w0] = _v_tarray; return false; } else if (std::holds_alternative<registry::TShared>((*type_value))) { auto _v_tshared = std::get<registry::TShared>((*type_value)); return false; } else if (std::holds_alternative<registry::TFn>((*type_value))) { auto _v_tfn = std::get<registry::TFn>((*type_value)); return false; } if (std::holds_alternative<registry::TAssoc>((*type_value))) { auto _v_tassoc = std::get<registry::TAssoc>((*type_value)); auto [_w0, _w1] = _v_tassoc; return false; } if (std::holds_alternative<registry::TTuple>((*type_value))) { auto _v_ttuple = std::get<registry::TTuple>((*type_value)); auto [parameter_types] = _v_ttuple; return parameter_types.all([registry](std::shared_ptr<registry::Type> entry) mutable { return type_is_send_safe(entry, registry); }); } if (std::holds_alternative<registry::TNamed>((*type_value))) { auto _v_tnamed = std::get<registry::TNamed>((*type_value)); auto [record_name] = _v_tnamed; return type_is_send_safe_named(record_name, registry); } if (std::holds_alternative<registry::TGeneric>((*type_value))) { auto _v_tgeneric = std::get<registry::TGeneric>((*type_value)); auto [type_name, type_arguments] = _v_tgeneric; return type_is_send_safe_generic(type_name, type_arguments, registry); } if (std::holds_alternative<registry::TPair>((*type_value))) { auto _v_tpair = std::get<registry::TPair>((*type_value)); auto [_w0, _w1] = _v_tpair; return false; } }();}

bool type_is_send_safe_field(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map, mlc::String field_name, registry::TypeRegistry registry) noexcept{return field_map.has(field_name) ? type_is_send_safe(field_map.get(field_name), registry) : false;}

bool type_is_send_safe_named(mlc::String record_name, registry::TypeRegistry registry) noexcept{
if (!registry::TypeRegistry_has_fields(registry, record_name)){
{
return false;
}
}
mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map = registry::TypeRegistry_fields_for(registry, record_name);
mlc::Array<mlc::String> field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, record_name);
return field_names.all([field_map, registry](mlc::String field_name) mutable { return type_is_send_safe_field(field_map, field_name, registry); });
}

bool type_is_send_safe_generic(mlc::String type_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
if (type_name == mlc::String("Option") && type_arguments.size() == 1){
{
return type_is_send_safe(type_arguments[0], registry);
}
}
if (type_name == mlc::String("Result") && type_arguments.size() == 2){
{
return type_is_send_safe(type_arguments[0], registry) && type_is_send_safe(type_arguments[1], registry);
}
}
if (type_name == mlc::String("Channel") || type_name == mlc::String("Task") || type_name == mlc::String("Map") || type_name == mlc::String("Shared") || type_name == mlc::String("Arc") || type_name == mlc::String("Mutex") || type_name == mlc::String("Future")){
{
return false;
}
}
if (type_arguments.size() == 0){
{
return false;
}
}
return type_arguments.all([registry](std::shared_ptr<registry::Type> argument) mutable { return type_is_send_safe(argument, registry); });
}

} // namespace send_safe
