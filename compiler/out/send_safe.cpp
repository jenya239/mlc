#define main mlc_user_main
#include "send_safe.hpp"

#include "registry.hpp"

namespace send_safe {

using namespace registry;

bool type_is_send(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const registry::TI32& tI32) { return true; },
[&](const registry::TString& tString) { return true; },
[&](const registry::TBool& tBool) { return true; },
[&](const registry::TUnit& tUnit) { return true; },
[&](const registry::TI64& tI64) { return true; },
[&](const registry::TF64& tF64) { return true; },
[&](const registry::TU8& tU8) { return true; },
[&](const registry::TUsize& tUsize) { return true; },
[&](const registry::TChar& tChar) { return true; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TTuple& tTuple) { auto [parameter_types] = tTuple; return parameter_types.all([=](std::shared_ptr<registry::Type> entry) mutable { return type_is_send(entry, registry); }); },
[&](const registry::TNamed& tNamed) { auto [record_name] = tNamed; return type_is_send_named(record_name, registry); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return type_is_send_generic(type_name, type_arguments, registry); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; }
}, (*type_value));
}
bool type_is_send_safe(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return type_is_send(type_value, registry);
}
bool type_is_sync(std::shared_ptr<registry::Type> type_value, registry::TypeRegistry registry) noexcept{
  return std::visit(overloaded{[&](const registry::TUnknown& tUnknown) { return true; },
[&](const registry::TI32& tI32) { return true; },
[&](const registry::TString& tString) { return true; },
[&](const registry::TBool& tBool) { return true; },
[&](const registry::TUnit& tUnit) { return true; },
[&](const registry::TI64& tI64) { return true; },
[&](const registry::TF64& tF64) { return true; },
[&](const registry::TU8& tU8) { return true; },
[&](const registry::TUsize& tUsize) { return true; },
[&](const registry::TChar& tChar) { return true; },
[&](const registry::TArray& tArray) { auto [__0] = tArray; return false; },
[&](const registry::TShared& tShared) { auto [__0] = tShared; return false; },
[&](const registry::TFn& tFn) { auto [__0, __1] = tFn; return false; },
[&](const registry::TAssoc& tAssoc) { auto [__0, __1] = tAssoc; return false; },
[&](const registry::TTuple& tTuple) { auto [parameter_types] = tTuple; return parameter_types.all([=](std::shared_ptr<registry::Type> entry) mutable { return type_is_sync(entry, registry); }); },
[&](const registry::TNamed& tNamed) { auto [record_name] = tNamed; return type_is_sync_named(record_name, registry); },
[&](const registry::TGeneric& tGeneric) { auto [type_name, type_arguments] = tGeneric; return type_is_sync_generic(type_name, type_arguments, registry); },
[&](const registry::TPair& tPair) { auto [__0, __1] = tPair; return false; }
}, (*type_value));
}
bool type_is_send_field(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map, mlc::String field_name, registry::TypeRegistry registry) noexcept{
  if (field_map.has(field_name))   {
    return type_is_send(field_map.get(field_name), registry);
  } else   {
    return false;
  }
}
bool type_is_send_named(mlc::String record_name, registry::TypeRegistry registry) noexcept{
  if ((!registry::TypeRegistry_has_fields(registry, record_name)))   {
    return false;
  }
  auto field_map = registry::TypeRegistry_fields_for(registry, record_name);
  auto field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, record_name);
  return field_names.all([=](mlc::String field_name) mutable { return type_is_send_field(field_map, field_name, registry); });
}
bool type_is_sync_field(mlc::HashMap<mlc::String, std::shared_ptr<registry::Type>> field_map, mlc::String field_name, registry::TypeRegistry registry) noexcept{
  if (field_map.has(field_name))   {
    return type_is_sync(field_map.get(field_name), registry);
  } else   {
    return false;
  }
}
bool type_is_sync_named(mlc::String record_name, registry::TypeRegistry registry) noexcept{
  if ((!registry::TypeRegistry_has_fields(registry, record_name)))   {
    return false;
  }
  auto field_map = registry::TypeRegistry_fields_for(registry, record_name);
  auto field_names = registry::TypeRegistry_record_field_names_ordered_for(registry, record_name);
  return field_names.all([=](mlc::String field_name) mutable { return type_is_sync_field(field_map, field_name, registry); });
}
bool type_is_send_generic(mlc::String type_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
  if (((type_name == mlc::String("Option", 6)) && (type_arguments.length() == 1)))   {
    return type_is_send(type_arguments[0], registry);
  }
  if (((type_name == mlc::String("Result", 6)) && (type_arguments.length() == 2)))   {
    return (type_is_send(type_arguments[0], registry) && type_is_send(type_arguments[1], registry));
  }
  if (((type_name == mlc::String("Arc", 3)) && (type_arguments.length() == 1)))   {
    return type_is_send(type_arguments[0], registry);
  }
  if (((type_name == mlc::String("Mutex", 5)) && (type_arguments.length() == 1)))   {
    return type_is_send(type_arguments[0], registry);
  }
  if (((((((type_name == mlc::String("Channel", 7)) || (type_name == mlc::String("Task", 4))) || (type_name == mlc::String("Map", 3))) || (type_name == mlc::String("Shared", 6))) || (type_name == mlc::String("Future", 6))) || (type_name == mlc::String("Weak", 4))))   {
    return false;
  }
  if ((type_arguments.length() == 0))   {
    return false;
  }
  return type_arguments.all([=](std::shared_ptr<registry::Type> argument) mutable { return type_is_send(argument, registry); });
}
bool type_is_sync_generic(mlc::String type_name, mlc::Array<std::shared_ptr<registry::Type>> type_arguments, registry::TypeRegistry registry) noexcept{
  if (((type_name == mlc::String("Option", 6)) && (type_arguments.length() == 1)))   {
    return type_is_sync(type_arguments[0], registry);
  }
  if (((type_name == mlc::String("Result", 6)) && (type_arguments.length() == 2)))   {
    return (type_is_sync(type_arguments[0], registry) && type_is_sync(type_arguments[1], registry));
  }
  if (((type_name == mlc::String("Mutex", 5)) && (type_arguments.length() == 1)))   {
    return true;
  }
  if (((type_name == mlc::String("Arc", 3)) && (type_arguments.length() == 1)))   {
    return type_is_sync(type_arguments[0], registry);
  }
  if (((((((type_name == mlc::String("Channel", 7)) || (type_name == mlc::String("Task", 4))) || (type_name == mlc::String("Map", 3))) || (type_name == mlc::String("Shared", 6))) || (type_name == mlc::String("Future", 6))) || (type_name == mlc::String("Weak", 4))))   {
    return false;
  }
  if ((type_arguments.length() == 0))   {
    return false;
  }
  return type_arguments.all([=](std::shared_ptr<registry::Type> argument) mutable { return type_is_sync(argument, registry); });
}

} // namespace send_safe
