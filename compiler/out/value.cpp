#include "value.hpp"

#include "ast.hpp"

namespace value {

using namespace ast;
using namespace ast_tokens;

value::VmValue vm_value_unit() noexcept;

bool vm_value_is_truthy(value::VmValue value) noexcept;

ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(value::VmValue value) noexcept;

int vm_value_int_or_zero(value::VmValue value) noexcept;

value::VmValue vm_value_unit() noexcept{return (value::VmUnit{});}

bool vm_value_is_truthy(value::VmValue value) noexcept{return std::visit(overloaded{
  [&](const VmI32& vmi32) -> bool { auto [number] = vmi32; return number != 0; },
  [&](const VmBool& vmbool) -> bool { auto [flag] = vmbool; return flag; },
  [&](const VmString& vmstring) -> bool { auto [text] = vmstring; return text.length() > 0; },
  [&](const VmArray& vmarray) -> bool { auto [array] = vmarray; return array.integer_elements.size() > 0; },
  [&](const VmMap& vmmap) -> bool { auto [map] = vmmap; return map.entry_count > 0; },
  [&](const VmVariant& vmvariant) -> bool { auto [_w0] = vmvariant; return true; },
  [&](const VmUnit& vmunit) -> bool { return false; }
}, value);}

ast::Result<int, mlc::Array<mlc::String>> vm_value_as_i32(value::VmValue value) noexcept{return [&]() -> ast::Result<int, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(value)) { auto _v_vmi32 = std::get<value::VmI32>(value); auto [number] = _v_vmi32; return ast::Ok<int>(number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: expected i32")}); }();}

int vm_value_int_or_zero(value::VmValue value) noexcept{return [&]() { if (std::holds_alternative<value::VmI32>(value)) { auto _v_vmi32 = std::get<value::VmI32>(value); auto [number] = _v_vmi32; return number; } return 0; }();}

} // namespace value
