#ifndef NATIVE_HPP
#define NATIVE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "value.hpp"
namespace native {

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

} // namespace native

#endif // NATIVE_HPP
