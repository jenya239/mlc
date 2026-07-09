#ifndef VALUE_HPP
#define VALUE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
namespace value {

struct VmI32;
struct VmBool;
struct VmString;
struct VmArray;
struct VmMap;
struct VmVariant;
struct VmUnit;
using VmValue = std::variant<VmI32, VmBool, VmString, VmArray, VmMap, VmVariant, VmUnit>;
struct VmArrayValue {
  mlc::Array<int> integer_elements;
};
struct VmMapValue {
  mlc::Array<mlc::String> keys;
  mlc::Array<int> integer_values;
  int entry_count;
};
struct VmVariantValue {
  mlc::String tag;
  mlc::Array<int> integer_fields;
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
struct VmUnit {};
VmValue vm_value_unit() noexcept;
bool vm_value_is_truthy(VmValue value) noexcept;
ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(VmValue value) noexcept;
int vm_value_int_or_zero(VmValue value) noexcept;

} // namespace value

#endif // VALUE_HPP
