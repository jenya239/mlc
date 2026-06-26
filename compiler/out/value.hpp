#ifndef VALUE_HPP
#define VALUE_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"

namespace value {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct VmArrayValue {mlc::Array<int> integer_elements;};

struct VmMapValue {mlc::Array<mlc::String> keys;mlc::Array<int> integer_values;int entry_count;};

struct VmVariantValue {mlc::String tag;mlc::Array<int> integer_fields;};

struct VmI32 {int field0;};struct VmBool {bool field0;};struct VmString {mlc::String field0;};struct VmArray {value::VmArrayValue field0;};struct VmMap {value::VmMapValue field0;};struct VmVariant {value::VmVariantValue field0;};struct VmUnit {};using VmValue = std::variant<VmI32, VmBool, VmString, VmArray, VmMap, VmVariant, VmUnit>;

value::VmValue vm_value_unit() noexcept;

bool vm_value_is_truthy(value::VmValue value) noexcept;

ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(value::VmValue value) noexcept;

int vm_value_int_or_zero(value::VmValue value) noexcept;

} // namespace value

#endif // VALUE_HPP
