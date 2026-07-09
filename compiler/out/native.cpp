#define main mlc_user_main
#include "native.hpp"

#include "ast.hpp"
#include "value.hpp"

namespace native {

using namespace ast;
using namespace value;
using namespace ast_tokens;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_call(mlc::String name, mlc::Array<value::VmValue> arguments) noexcept{
  if ((name == mlc::String("println", 7)))   {
    return vm_native_println(arguments);
  } else if ((name == mlc::String("__mir_array_empty", 17)))   {
    return vm_native_array_empty(arguments);
  } else if ((name == mlc::String("__mir_array_push", 16)))   {
    return vm_native_array_push(arguments);
  } else if ((name == mlc::String("__mir_array_length", 18)))   {
    return vm_native_array_length(arguments);
  } else if ((name == mlc::String("__mir_array_get", 15)))   {
    return vm_native_array_get(arguments);
  } else if ((name == mlc::String("__mir_map_empty", 15)))   {
    return vm_native_map_empty(arguments);
  } else if ((name == mlc::String("__mir_map_set", 13)))   {
    return vm_native_map_set(arguments);
  } else if ((name == mlc::String("__mir_map_get", 13)))   {
    return vm_native_map_get(arguments);
  } else if ((name == mlc::String("__mir_map_length", 16)))   {
    return vm_native_map_length(arguments);
  } else if ((name == mlc::String("__mir_string_length", 19)))   {
    return vm_native_string_length(arguments);
  } else if ((name == mlc::String("__mir_length", 12)))   {
    return vm_native_collection_length(arguments);
  } else if ((name == mlc::String("__mir_variant_is", 16)))   {
    return vm_native_variant_is(arguments);
  } else if ((name == mlc::String("__mir_variant_field", 19)))   {
    return vm_native_variant_field(arguments);
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: unknown native ", 19) + mlc::to_string(name)) + mlc::String("", 0))}};
  }
}
value::VmValue vm_variant_ctor(mlc::String tag, mlc::Array<value::VmValue> arguments) noexcept{
  auto integer_fields = mlc::Array<int>{};
  auto index = 0;
  while ((index < arguments.length()))   {
    integer_fields.push_back(value::vm_value_int_or_zero(arguments[index]));
    (index = (index + 1));
  }
  return value::VmVariant{value::VmVariantValue{tag, integer_fields}};
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_println(mlc::Array<value::VmValue> arguments) noexcept{
  auto index = 0;
  while ((index < arguments.length()))   {
    std::visit(overloaded{[&](const value::VmI32& vmI32) -> void { auto [integer] = vmI32; mlc::io::println(((mlc::String("", 0) + mlc::to_string(integer)) + mlc::String("", 0))); },
[&](const value::VmBool& vmBool) -> void { auto [flag] = vmBool; mlc::io::println((flag ? (mlc::String("true", 4)) : (mlc::String("false", 5)))); },
[&](const value::VmString& vmString) -> void { auto [text] = vmString; mlc::io::println(text); },
[&](const value::VmArray& vmArray) -> void { auto [__0] = vmArray; mlc::io::println(mlc::String("<array>", 7)); },
[&](const value::VmMap& vmMap) -> void { auto [__0] = vmMap; mlc::io::println(mlc::String("<map>", 5)); },
[&](const value::VmVariant& vmVariant) -> void { auto [variant] = vmVariant; mlc::io::println(((mlc::String("<variant ", 9) + mlc::to_string(variant.tag)) + mlc::String(">", 1))); },
[&](const value::VmUnit& vmUnit) -> void { mlc::io::println(mlc::String("()", 2)); }
}, arguments[index]);
    (index = (index + 1));
  }
  return ast::Ok<value::VmValue>{value::vm_value_unit()};
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_empty(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() == 0))   {
    return ast::Ok<value::VmValue>{value::VmArray{value::VmArrayValue{{}}}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_empty expects no arguments", 42)}};
  }
}
ast::Result<int, mlc::Array<mlc::String>> vm_native_value_as_i32(value::VmValue value) noexcept{
  return [&]() -> ast::Result<int, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [number] = vmI32; return ast::Ok<int>{number};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: expected i32 value", 22)}};
std::abort();
}();
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_push(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 2))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_push expects 2 arguments", 40)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmArray>(__match_subject)) {
const value::VmArray& vmArray = std::get<value::VmArray>(__match_subject);
auto [array] = vmArray; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [element] = ok; return [&]() {
auto integer_elements = array.integer_elements;
integer_elements.push_back(element);
return ast::Ok<value::VmValue>{value::VmArray{value::VmArrayValue{integer_elements}}};
}(); }
}, vm_native_value_as_i32(arguments[1]));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_push expects array", 34)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_length(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 1))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects 1 argument", 41)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmArray>(__match_subject)) {
const value::VmArray& vmArray = std::get<value::VmArray>(__match_subject);
auto [array] = vmArray; return ast::Ok<value::VmValue>{value::VmI32{array.integer_elements.length()}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects array", 36)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_at(value::VmArrayValue array, int index) noexcept{
  if (((index >= 0) && (index < array.integer_elements.length())))   {
    return ast::Ok<value::VmValue>{value::VmI32{array.integer_elements[index]}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: array index out of bounds", 29)}};
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_get(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 2))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_get expects 2 arguments", 39)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmArray>(__match_subject)) {
const value::VmArray& vmArray = std::get<value::VmArray>(__match_subject);
auto [array] = vmArray; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [index] = ok; return vm_native_array_at(array, index); }
}, vm_native_value_as_i32(arguments[1]));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_get expects array", 33)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_empty(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() == 0))   {
    return ast::Ok<value::VmValue>{value::VmMap{value::VmMapValue{{}, {}, 0}}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_empty expects no arguments", 40)}};
  }
}
int vm_native_map_find_key_index(mlc::Array<mlc::String> keys, mlc::String key) noexcept{
  auto index = 0;
  while ((index < keys.length()))   {
    if ((keys[index] == key))     {
      return index;
    }
    (index = (index + 1));
  }
  return (-1);
}
mlc::Array<int> vm_native_map_replace_value(mlc::Array<int> integer_values, int index, int value) noexcept{
  auto rebuilt = mlc::Array<int>{};
  auto position = 0;
  while ((position < integer_values.length()))   {
    if ((position == index))     {
      rebuilt.push_back(value);
    } else     {
      rebuilt.push_back(integer_values[position]);
    }
    (position = (position + 1));
  }
  return rebuilt;
}
value::VmValue vm_native_map_lookup(value::VmMapValue map_value, mlc::String key) noexcept{
  auto existing_index = vm_native_map_find_key_index(map_value.keys, key);
  if ((existing_index >= 0))   {
    return value::VmI32{map_value.integer_values[existing_index]};
  } else   {
    return value::VmI32{0};
  }
}
value::VmValue vm_native_map_insert_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept{
  auto keys = map_value.keys;
  auto integer_values = map_value.integer_values;
  keys.push_back(key);
  integer_values.push_back(integer_value);
  return value::VmMap{value::VmMapValue{keys, integer_values, (map_value.entry_count + 1)}};
}
value::VmValue vm_native_map_update_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept{
  auto existing_index = vm_native_map_find_key_index(map_value.keys, key);
  return value::VmMap{value::VmMapValue{map_value.keys, vm_native_map_replace_value(map_value.integer_values, existing_index, integer_value), map_value.entry_count}};
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_set(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 3))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects 3 arguments", 37)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmMap>(__match_subject)) {
const value::VmMap& vmMap = std::get<value::VmMap>(__match_subject);
auto [map_value] = vmMap; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[1];
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [key] = vmString; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [integer_value] = ok; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
  if ((vm_native_map_find_key_index(map_value.keys, key) >= 0))   {
    return ast::Ok<value::VmValue>{vm_native_map_update_value(map_value, key, integer_value)};
  } else   {
    return ast::Ok<value::VmValue>{vm_native_map_insert_value(map_value, key, integer_value)};
  }
}(); }
}, vm_native_value_as_i32(arguments[2]));
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects string key", 36)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects map", 29)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_get(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 2))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects 2 arguments", 37)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmMap>(__match_subject)) {
const value::VmMap& vmMap = std::get<value::VmMap>(__match_subject);
auto [map_value] = vmMap; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[1];
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [key] = vmString; return ast::Ok<value::VmValue>{vm_native_map_lookup(map_value, key)};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects string key", 36)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects map", 29)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_length(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 1))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects 1 argument", 41)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmMap>(__match_subject)) {
const value::VmMap& vmMap = std::get<value::VmMap>(__match_subject);
auto [map] = vmMap; return ast::Ok<value::VmValue>{value::VmI32{map.entry_count}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_map_length expects map", 32)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_string_length(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 1))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_string_length expects 1 argument", 42)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [text] = vmString; return ast::Ok<value::VmValue>{value::VmI32{text.length()}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_string_length expects string", 38)}};
std::abort();
}();
  }
}
ast::Result<value::VmVariantValue, mlc::Array<mlc::String>> vm_native_variant_value(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() == 0))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: variant native expects value argument", 41)}};
  } else   {
    return [&]() -> ast::Result<value::VmVariantValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmVariant>(__match_subject)) {
const value::VmVariant& vmVariant = std::get<value::VmVariant>(__match_subject);
auto [variant] = vmVariant; return ast::Ok<value::VmVariantValue>{variant};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: expected variant value", 26)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_is(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 2))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects 2 arguments", 40)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmVariant>(__match_subject)) {
const value::VmVariant& vmVariant = std::get<value::VmVariant>(__match_subject);
auto [variant] = vmVariant; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[1];
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [expected_tag] = vmString; return ast::Ok<value::VmValue>{value::VmBool{(variant.tag == expected_tag)}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects string tag", 39)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects variant", 36)}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_field(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 2))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_field expects 2 arguments", 43)}};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<value::VmVariantValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [variant] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [field_index] = ok; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
  if (((field_index >= 0) && (field_index < variant.integer_fields.length())))   {
    return ast::Ok<value::VmValue>{value::VmI32{variant.integer_fields[field_index]}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: variant field index out of bounds", 37)}};
  }
}(); }
}, vm_native_value_as_i32(arguments[1])); }
}, vm_native_variant_value(arguments));
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_collection_length(mlc::Array<value::VmValue> arguments) noexcept{
  if ((arguments.length() != 1))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_length expects 1 argument", 35)}};
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = arguments[0];
if (std::holds_alternative<value::VmArray>(__match_subject)) {
const value::VmArray& vmArray = std::get<value::VmArray>(__match_subject);
auto [array] = vmArray; return ast::Ok<value::VmValue>{value::VmI32{array.integer_elements.length()}};
}
if (std::holds_alternative<value::VmMap>(__match_subject)) {
const value::VmMap& vmMap = std::get<value::VmMap>(__match_subject);
auto [map] = vmMap; return ast::Ok<value::VmValue>{value::VmI32{map.entry_count}};
}
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [text] = vmString; return ast::Ok<value::VmValue>{value::VmI32{text.length()}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: __mir_length expects array, map, or string", 46)}};
std::abort();
}();
  }
}

} // namespace native
