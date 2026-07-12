#ifndef VALUE_HPP
#define VALUE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace value {

struct VmFieldI32;
struct VmFieldBool;
struct VmFieldString;
struct VmFieldVariant;
struct VmFieldRecord;
struct VmFieldArray;
struct VmFieldMap;
using VmFieldSlot = std::variant<VmFieldI32, VmFieldBool, VmFieldString, VmFieldVariant, VmFieldRecord, VmFieldArray, VmFieldMap>;
struct VmI32;
struct VmBool;
struct VmString;
struct VmArray;
struct VmMap;
struct VmVariant;
struct VmRecord;
struct VmUnit;
using VmValue = std::variant<VmI32, VmBool, VmString, VmArray, VmMap, VmVariant, VmRecord, VmUnit>;
struct VmArrayValue {
  mlc::Array<VmFieldSlot> elements;
};
struct VmMapValue {
  mlc::Array<mlc::String> keys;
  mlc::Array<VmFieldSlot> values;
  int entry_count;
};
struct VmVariantValue {
  mlc::String tag;
  mlc::Array<VmFieldSlot> fields;
};
struct VmRecordValue {
  mlc::String type_name;
  mlc::Array<mlc::String> field_names;
  mlc::Array<VmFieldSlot> fields;
};
struct VmFieldI32 {
  int field0;
};
struct VmFieldBool {
  bool field0;
};
struct VmFieldString {
  mlc::String field0;
};
struct VmFieldVariant {
  std::shared_ptr<VmVariantValue> field0;
};
struct VmFieldRecord {
  std::shared_ptr<VmRecordValue> field0;
};
struct VmFieldArray {
  std::shared_ptr<VmArrayValue> field0;
};
struct VmFieldMap {
  std::shared_ptr<VmMapValue> field0;
};
struct VmI32 {
  int field0;
};
struct VmBool {
  bool field0;
};
struct VmString {
  mlc::String field0;
};
struct VmArray {
  VmArrayValue field0;
};
struct VmMap {
  VmMapValue field0;
};
struct VmVariant {
  VmVariantValue field0;
};
struct VmRecord {
  VmRecordValue field0;
};
struct VmUnit {};
VmValue vm_value_unit() noexcept;
bool vm_value_is_truthy(VmValue value) noexcept;
ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(VmValue value) noexcept;
int vm_value_int_or_zero(VmValue value) noexcept;
ast::Result<VmFieldSlot, mlc::Array<mlc::String>> vm_field_slot_from_value(VmValue value) noexcept;
VmValue vm_value_from_field_slot(VmFieldSlot slot) noexcept;

} // namespace value

#endif // VALUE_HPP
