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
[&](const VmArray& vmArray) { auto [array] = vmArray; return (array.integer_elements.length() > 0); },
[&](const VmMap& vmMap) { auto [map] = vmMap; return (map.entry_count > 0); },
[&](const VmVariant& vmVariant) { auto [__0] = vmVariant; return true; },
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

} // namespace value
