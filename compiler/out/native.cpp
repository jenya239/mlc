#include "native.hpp"

#include "ast.hpp"
#include "value.hpp"

namespace native {

using namespace ast;
using namespace value;
using namespace ast_tokens;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_call(mlc::String name, mlc::Array<value::VmValue> arguments) noexcept;

value::VmValue vm_variant_ctor(mlc::String tag, mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_println(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_empty(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<int, mlc::Array<mlc::String>> vm_native_value_as_i32(value::VmValue value) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_push(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_length(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_at(value::VmArrayValue array, int index) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_get(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_empty(mlc::Array<value::VmValue> arguments) noexcept;

int vm_native_map_find_key_index(mlc::Array<mlc::String> keys, mlc::String key) noexcept;

mlc::Array<int> vm_native_map_replace_value(mlc::Array<int> integer_values, int index, int value) noexcept;

value::VmValue vm_native_map_lookup(value::VmMapValue map_value, mlc::String key) noexcept;

value::VmValue vm_native_map_insert_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept;

value::VmValue vm_native_map_update_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_set(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_get(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_length(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_string_length(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmVariantValue, mlc::Array<mlc::String>> vm_native_variant_value(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_is(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_field(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_collection_length(mlc::Array<value::VmValue> arguments) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_call(mlc::String name, mlc::Array<value::VmValue> arguments) noexcept{return name == mlc::String("println") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_println(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_array_empty") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_array_empty(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_array_push") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_array_push(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_array_length") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_array_length(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_array_get") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_array_get(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_map_empty") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_map_empty(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_map_set") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_map_set(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_map_get") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_map_get(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_map_length") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_map_length(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_string_length") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_string_length(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_length") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_collection_length(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_variant_is") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_variant_is(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(name == mlc::String("__mir_variant_field") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(vm_native_variant_field(arguments)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unknown native ") + name}))))))))))))));}

value::VmValue vm_variant_ctor(mlc::String tag, mlc::Array<value::VmValue> arguments) noexcept{
mlc::Array<int> integer_fields = {};
int index = 0;
while (index < arguments.size()){
{
integer_fields.push_back(value::vm_value_int_or_zero(arguments[index]));
index = index + 1;
}
}
return value::VmVariant(value::VmVariantValue{tag, integer_fields});
}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_println(mlc::Array<value::VmValue> arguments) noexcept{
int index = 0;
while (index < arguments.size()){
{
std::visit(overloaded{
  [&](const VmI32& vmi32) -> int { auto [integer] = vmi32; return mlc::io::println(mlc::to_string(integer)); },
  [&](const VmBool& vmbool) -> int { auto [flag] = vmbool; return mlc::io::println(flag ? mlc::String("true") : mlc::String("false")); },
  [&](const VmString& vmstring) -> int { auto [text] = vmstring; return mlc::io::println(text); },
  [&](const VmArray& vmarray) -> int { auto [_w0] = vmarray; return mlc::io::println(mlc::String("<array>")); },
  [&](const VmMap& vmmap) -> int { auto [_w0] = vmmap; return mlc::io::println(mlc::String("<map>")); },
  [&](const VmVariant& vmvariant) -> int { auto [variant] = vmvariant; return mlc::io::println(mlc::String("<variant ") + variant.tag + mlc::String(">")); },
  [&](const VmUnit& vmunit) -> int { return mlc::io::println(mlc::String("()")); }
}, arguments[index]);
index = index + 1;
}
}
return ast::Ok<value::VmValue>(value::vm_value_unit());
}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_empty(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() == 0 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmArray(value::VmArrayValue{{}}))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_empty expects no arguments")}));}

ast::Result<int, mlc::Array<mlc::String>> vm_native_value_as_i32(value::VmValue value) noexcept{return [&]() -> ast::Result<int, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(value)) { auto _v_vmi32 = std::get<value::VmI32>(value); auto [number] = _v_vmi32; return ast::Ok<int>(number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: expected i32 value")}); }();}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_push(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 2 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_push expects 2 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmArray>(arguments[0])) { auto _v_vmarray = std::get<value::VmArray>(arguments[0]); auto [array] = _v_vmarray; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [element] = ok; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { 
  mlc::Array<int> integer_elements = array.integer_elements;
  integer_elements.push_back(element);
  return ast::Ok<value::VmValue>(value::VmArray(value::VmArrayValue{integer_elements}));
 }(); }
}, vm_native_value_as_i32(arguments[1])); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_push expects array")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_length(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 1 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects 1 argument")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmArray>(arguments[0])) { auto _v_vmarray = std::get<value::VmArray>(arguments[0]); auto [array] = _v_vmarray; return ast::Ok<value::VmValue>(value::VmI32(array.integer_elements.size())); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects array")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_at(value::VmArrayValue array, int index) noexcept{return index >= 0 && index < array.integer_elements.size() ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(array.integer_elements[index]))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: array index out of bounds")}));}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_array_get(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 2 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_get expects 2 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmArray>(arguments[0])) { auto _v_vmarray = std::get<value::VmArray>(arguments[0]); auto [array] = _v_vmarray; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [index] = ok; return vm_native_array_at(array, index); }
}, vm_native_value_as_i32(arguments[1])); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_get expects array")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_empty(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() == 0 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmMap(value::VmMapValue{{}, {}, 0}))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_empty expects no arguments")}));}

int vm_native_map_find_key_index(mlc::Array<mlc::String> keys, mlc::String key) noexcept{
int index = 0;
while (index < keys.size()){
{
if (keys[index] == key){
{
return index;
}
}
index = index + 1;
}
}
return -1;
}

mlc::Array<int> vm_native_map_replace_value(mlc::Array<int> integer_values, int index, int value) noexcept{
mlc::Array<int> rebuilt = {};
int position = 0;
while (position < integer_values.size()){
{
if (position == index){
{
rebuilt.push_back(value);
}
} else {
{
rebuilt.push_back(integer_values[position]);
}
}
position = position + 1;
}
}
return rebuilt;
}

value::VmValue vm_native_map_lookup(value::VmMapValue map_value, mlc::String key) noexcept{
int existing_index = vm_native_map_find_key_index(map_value.keys, key);
return existing_index >= 0 ? value::VmValue(value::VmI32(map_value.integer_values[existing_index])) : value::VmValue(value::VmI32(0));
}

value::VmValue vm_native_map_insert_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept{
mlc::Array<mlc::String> keys = map_value.keys;
mlc::Array<int> integer_values = map_value.integer_values;
keys.push_back(key);
integer_values.push_back(integer_value);
return value::VmMap(value::VmMapValue{keys, integer_values, map_value.entry_count + 1});
}

value::VmValue vm_native_map_update_value(value::VmMapValue map_value, mlc::String key, int integer_value) noexcept{
int existing_index = vm_native_map_find_key_index(map_value.keys, key);
return value::VmMap(value::VmMapValue{map_value.keys, vm_native_map_replace_value(map_value.integer_values, existing_index, integer_value), map_value.entry_count});
}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_set(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 3 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects 3 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmMap>(arguments[0])) { auto _v_vmmap = std::get<value::VmMap>(arguments[0]); auto [map_value] = _v_vmmap; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(arguments[1])) { auto _v_vmstring = std::get<value::VmString>(arguments[1]); auto [key] = _v_vmstring; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [integer_value] = ok; return vm_native_map_find_key_index(map_value.keys, key) >= 0 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(vm_native_map_update_value(map_value, key, integer_value))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(vm_native_map_insert_value(map_value, key, integer_value))); }
}, vm_native_value_as_i32(arguments[2])); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects string key")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_set expects map")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_get(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 2 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects 2 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmMap>(arguments[0])) { auto _v_vmmap = std::get<value::VmMap>(arguments[0]); auto [map_value] = _v_vmmap; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(arguments[1])) { auto _v_vmstring = std::get<value::VmString>(arguments[1]); auto [key] = _v_vmstring; return ast::Ok<value::VmValue>(vm_native_map_lookup(map_value, key)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects string key")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_get expects map")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_map_length(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 1 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_array_length expects 1 argument")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmMap>(arguments[0])) { auto _v_vmmap = std::get<value::VmMap>(arguments[0]); auto [map] = _v_vmmap; return ast::Ok<value::VmValue>(value::VmI32(map.entry_count)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_map_length expects map")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_string_length(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 1 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_string_length expects 1 argument")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(arguments[0])) { auto _v_vmstring = std::get<value::VmString>(arguments[0]); auto [text] = _v_vmstring; return ast::Ok<value::VmValue>(value::VmI32(text.length())); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_string_length expects string")}); }());}

ast::Result<value::VmVariantValue, mlc::Array<mlc::String>> vm_native_variant_value(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() == 0 ? ast::Result<value::VmVariantValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: variant native expects value argument")})) : ast::Result<value::VmVariantValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmVariantValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmVariant>(arguments[0])) { auto _v_vmvariant = std::get<value::VmVariant>(arguments[0]); auto [variant] = _v_vmvariant; return ast::Ok<value::VmVariantValue>(variant); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: expected variant value")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_is(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 2 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects 2 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmVariant>(arguments[0])) { auto _v_vmvariant = std::get<value::VmVariant>(arguments[0]); auto [variant] = _v_vmvariant; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(arguments[1])) { auto _v_vmstring = std::get<value::VmString>(arguments[1]); auto [expected_tag] = _v_vmstring; return ast::Ok<value::VmValue>(value::VmBool(variant.tag == expected_tag)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects string tag")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_is expects variant")}); }());}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_variant_field(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 2 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_variant_field expects 2 arguments")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<value::VmVariantValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [variant] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<int>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [field_index] = ok; return field_index >= 0 && field_index < variant.integer_fields.size() ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(variant.integer_fields[field_index]))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: variant field index out of bounds")})); }
}, vm_native_value_as_i32(arguments[1])); }
}, vm_native_variant_value(arguments)));}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_native_collection_length(mlc::Array<value::VmValue> arguments) noexcept{return arguments.size() != 1 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_length expects 1 argument")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmArray>(arguments[0])) { auto _v_vmarray = std::get<value::VmArray>(arguments[0]); auto [array] = _v_vmarray; return ast::Ok<value::VmValue>(value::VmI32(array.integer_elements.size())); } if (std::holds_alternative<value::VmMap>(arguments[0])) { auto _v_vmmap = std::get<value::VmMap>(arguments[0]); auto [map] = _v_vmmap; return ast::Ok<value::VmValue>(value::VmI32(map.entry_count)); } if (std::holds_alternative<value::VmString>(arguments[0])) { auto _v_vmstring = std::get<value::VmString>(arguments[0]); auto [text] = _v_vmstring; return ast::Ok<value::VmValue>(value::VmI32(text.length())); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: __mir_length expects array, map, or string")}); }());}

} // namespace native
