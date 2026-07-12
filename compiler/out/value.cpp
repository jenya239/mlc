#define main mlc_user_main
#include "value.hpp"

#include "ast.hpp"

namespace value {

using namespace ast;
using namespace ast_tokens;

VmValue vm_value_unit() noexcept{
  return VmUnit{};
}
bool vm_value_is_truthy(VmValue value) noexcept{
  return std::visit(overloaded{[&](const VmI32& vmI32) { auto [number] = vmI32; return (number != 0); },
[&](const VmBool& vmBool) { auto [flag] = vmBool; return flag; },
[&](const VmString& vmString) { auto [text] = vmString; return (text.length() > 0); },
[&](const VmArray& vmArray) { auto [array] = vmArray; return (array.elements.length() > 0); },
[&](const VmMap& vmMap) { auto [map] = vmMap; return (map.entry_count > 0); },
[&](const VmVariant& vmVariant) { auto [__0] = vmVariant; return true; },
[&](const VmRecord& vmRecord) { auto [__0] = vmRecord; return true; },
[&](const VmUnit& vmUnit) { return false; }
}, value);
}
ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(VmValue value) noexcept{
  return [&]() -> ast::Result<int, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<VmI32>(__match_subject)) {
const VmI32& vmI32 = std::get<VmI32>(__match_subject);
auto [number] = vmI32; return ast::Ok<int>{number};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: expected i32", 16)}};
std::abort();
}();
}
int vm_value_int_or_zero(VmValue value) noexcept{
  return [&]() -> int {
auto __match_subject = value;
if (std::holds_alternative<VmI32>(__match_subject)) {
const VmI32& vmI32 = std::get<VmI32>(__match_subject);
auto [number] = vmI32; return number;
}
return 0;
std::abort();
}();
}
ast::Result<VmFieldSlot, mlc::Array<mlc::String>> vm_field_slot_from_value(VmValue value) noexcept{
  return std::visit(overloaded{[&](const VmI32& vmI32) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [number] = vmI32; return ast::Ok<VmFieldSlot>{VmFieldI32{number}}; },
[&](const VmBool& vmBool) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [flag] = vmBool; return ast::Ok<VmFieldSlot>{VmFieldBool{flag}}; },
[&](const VmString& vmString) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [text] = vmString; return ast::Ok<VmFieldSlot>{VmFieldString{text}}; },
[&](const VmVariant& vmVariant) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [variant] = vmVariant; return ast::Ok<VmFieldSlot>{VmFieldVariant{std::make_shared<VmVariantValue>(variant)}}; },
[&](const VmRecord& vmRecord) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [record] = vmRecord; return ast::Ok<VmFieldSlot>{VmFieldRecord{std::make_shared<VmRecordValue>(record)}}; },
[&](const VmArray& vmArray) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [array] = vmArray; return ast::Ok<VmFieldSlot>{VmFieldArray{std::make_shared<VmArrayValue>(array)}}; },
[&](const VmMap& vmMap) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { auto [map] = vmMap; return ast::Ok<VmFieldSlot>{VmFieldMap{std::make_shared<VmMapValue>(map)}}; },
[&](const VmUnit& vmUnit) -> ast::Result<VmFieldSlot, mlc::Array<mlc::String>> { return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: cannot store unit in field slot", 35)}}; }
}, value);
}
VmValue vm_value_from_field_slot(VmFieldSlot slot) noexcept{
  return std::visit(overloaded{[&](const VmFieldI32& vmFieldI32) -> VmValue { auto [number] = vmFieldI32; return VmI32{number}; },
[&](const VmFieldBool& vmFieldBool) -> VmValue { auto [flag] = vmFieldBool; return VmBool{flag}; },
[&](const VmFieldString& vmFieldString) -> VmValue { auto [text] = vmFieldString; return VmString{text}; },
[&](const VmFieldVariant& vmFieldVariant) -> VmValue { auto [variant_shared] = vmFieldVariant; return VmVariant{VmVariantValue{variant_shared->tag, variant_shared->fields}}; },
[&](const VmFieldRecord& vmFieldRecord) -> VmValue { auto [record_shared] = vmFieldRecord; return VmRecord{VmRecordValue{record_shared->type_name, record_shared->field_names, record_shared->fields}}; },
[&](const VmFieldArray& vmFieldArray) -> VmValue { auto [array_shared] = vmFieldArray; return VmArray{VmArrayValue{array_shared->elements}}; },
[&](const VmFieldMap& vmFieldMap) -> VmValue { auto [map_shared] = vmFieldMap; return VmMap{VmMapValue{map_shared->keys, map_shared->values, map_shared->entry_count}}; }
}, slot);
}

} // namespace value
