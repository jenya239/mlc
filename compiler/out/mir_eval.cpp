#define main mlc_user_main
#include "mir_eval.hpp"

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"

namespace mir_eval {

using namespace ast;
using namespace mir_types;
using namespace value;
using namespace frame;
using namespace ast_tokens;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirOperandLocal& mirOperandLocal) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [local_id] = mirOperandLocal; return frame::vm_locals_load(frame.locals, local_id); },
[&](const mir_types::MirOperandConstInt& mirOperandConstInt) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirOperandConstInt; return ast::Ok<value::VmValue>{value::VmI32{value}}; },
[&](const mir_types::MirOperandConstBool& mirOperandConstBool) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirOperandConstBool; return ast::Ok<value::VmValue>{value::VmBool{value}}; },
[&](const mir_types::MirOperandConstStr& mirOperandConstStr) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = mirOperandConstStr; return ast::Ok<value::VmValue>{value::VmString{value}}; },
[&](const mir_types::MirOperandUnit& mirOperandUnit) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Ok<value::VmValue>{value::vm_value_unit()}; }
}, operand);
}
bool vm_values_equal(value::VmValue left, value::VmValue right) noexcept{
  return std::visit(overloaded{[&](const value::VmI32& vmI32) { auto [left_number] = vmI32; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [right_number] = vmI32; return (left_number == right_number);
}
return false;
std::abort();
}(); },
[&](const value::VmBool& vmBool) { auto [left_flag] = vmBool; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<value::VmBool>(__match_subject)) {
const value::VmBool& vmBool = std::get<value::VmBool>(__match_subject);
auto [right_flag] = vmBool; return (left_flag == right_flag);
}
return false;
std::abort();
}(); },
[&](const value::VmString& vmString) { auto [left_text] = vmString; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [right_text] = vmString; return (left_text == right_text);
}
return false;
std::abort();
}(); },
[&](const value::VmArray& vmArray) { auto [left_array] = vmArray; return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<value::VmArray>(__match_subject)) {
const value::VmArray& vmArray = std::get<value::VmArray>(__match_subject);
auto [right_array] = vmArray; return (left_array.integer_elements.length() == right_array.integer_elements.length());
}
return false;
std::abort();
}(); },
[&](const value::VmMap& vmMap) { auto [__0] = vmMap; return false; },
[&](const value::VmVariant& vmVariant) { auto [__0] = vmVariant; return false; },
[&](const value::VmRecord& vmRecord) { auto [__0] = vmRecord; return false; },
[&](const value::VmUnit& vmUnit) { return [&]() -> bool {
auto __match_subject = right;
if (std::holds_alternative<value::VmUnit>(__match_subject)) {
const value::VmUnit& vmUnit = std::get<value::VmUnit>(__match_subject);
return true;
}
return false;
std::abort();
}(); }
}, left);
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary_i32(mlc::String operation, int left, int right) noexcept{
  if ((operation == mlc::String("+", 1)))   {
    return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_add(left, right)}};
  } else if ((operation == mlc::String("-", 1)))   {
    return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_sub(left, right)}};
  } else if ((operation == mlc::String("*", 1)))   {
    return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_mul(left, right)}};
  } else if (((operation == mlc::String("/", 1)) && (right != 0)))   {
    return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_div(left, right)}};
  } else if ((operation == mlc::String("/", 1)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: division by zero", 20)}};
  } else if (((operation == mlc::String("%", 1)) && (right != 0)))   {
    return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_mod(left, right)}};
  } else if ((operation == mlc::String("%", 1)))   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: modulo by zero", 18)}};
  } else if ((operation == mlc::String(">", 1)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left > right)}};
  } else if ((operation == mlc::String("<", 1)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left < right)}};
  } else if ((operation == mlc::String(">=", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left >= right)}};
  } else if ((operation == mlc::String("<=", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left <= right)}};
  } else if ((operation == mlc::String("==", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left == right)}};
  } else if ((operation == mlc::String("!=", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(left != right)}};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: unknown int binary ", 23) + mlc::to_string(operation)) + mlc::String("", 0))}};
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept{
  if ((operation == mlc::String("&&", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(value::vm_value_is_truthy(left) && value::vm_value_is_truthy(right))}};
  } else if ((operation == mlc::String("||", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(value::vm_value_is_truthy(left) || value::vm_value_is_truthy(right))}};
  } else if ((operation == mlc::String("==", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{vm_values_equal(left, right)}};
  } else if ((operation == mlc::String("!=", 2)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(!vm_values_equal(left, right))}};
  } else if ((operation == mlc::String("+", 1)))   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = left;
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [left_text] = vmString; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = right;
if (std::holds_alternative<value::VmString>(__match_subject)) {
const value::VmString& vmString = std::get<value::VmString>(__match_subject);
auto [right_text] = vmString; return ast::Ok<value::VmValue>{value::VmString{(left_text + right_text)}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: string concat requires string operand", 41)}};
std::abort();
}();
}
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [left_number] = vmI32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = right;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [right_number] = vmI32; return vm_eval_binary_i32(operation, left_number, right_number);
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand", 43)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: unsupported binary + operand", 32)}};
std::abort();
}();
  } else   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = left;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [left_number] = vmI32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = right;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [right_number] = vmI32; return vm_eval_binary_i32(operation, left_number, right_number);
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand", 43)}};
std::abort();
}();
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: unsupported binary ", 23) + mlc::to_string(operation)) + mlc::String("", 0))}};
std::abort();
}();
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_unary(mlc::String operation, value::VmValue value) noexcept{
  if ((operation == mlc::String("!", 1)))   {
    return ast::Ok<value::VmValue>{value::VmBool{(!value::vm_value_is_truthy(value))}};
  } else if ((operation == mlc::String("-", 1)))   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [number] = vmI32; return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_sub(0, number)}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: unary - requires i32", 24)}};
std::abort();
}();
  } else if ((operation == mlc::String("+", 1)))   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [number] = vmI32; return ast::Ok<value::VmValue>{value::VmI32{number}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: unary + requires i32", 24)}};
std::abort();
}();
  } else if ((operation == mlc::String("~", 1)))   {
    return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> {
auto __match_subject = value;
if (std::holds_alternative<value::VmI32>(__match_subject)) {
const value::VmI32& vmI32 = std::get<value::VmI32>(__match_subject);
auto [number] = vmI32; return ast::Ok<value::VmValue>{value::VmI32{mlc::arith::checked_sub(mlc::arith::checked_sub(0, number), 1)}};
}
return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{mlc::String("vm: unary ~ requires i32", 24)}};
std::abort();
}();
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: unknown unary ", 18) + mlc::to_string(operation)) + mlc::String("", 0))}};
  }
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirRvalueUse& mirRvalueUse) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [operand] = mirRvalueUse; return vm_eval_operand(frame, operand); },
[&](const mir_types::MirRvalueBinary& mirRvalueBinary) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [operation, left_operand, right_operand] = mirRvalueBinary; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [left_value] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [right_value] = ok; return vm_eval_binary(operation, left_value, right_value); }
}, vm_eval_operand(frame, right_operand)); }
}, vm_eval_operand(frame, left_operand)); },
[&](const mir_types::MirRvalueUnary& mirRvalueUnary) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [operation, operand] = mirRvalueUnary; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = ok; return vm_eval_unary(operation, value); }
}, vm_eval_operand(frame, operand)); }
}, rvalue);
}
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept{
  if ((index >= operands.length()))   {
    return ast::Ok<mlc::Array<value::VmValue>>{values};
  } else   {
    return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [value] = ok; return vm_eval_operands_loop(frame, operands, mlc::arith::checked_add(index, 1), vm_values_push(values, value)); }
}, vm_eval_operand(frame, operands[index]));
  }
}
mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept{
  auto next_values = values;
  next_values.push_back(value);
  return next_values;
}
ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept{
  return vm_eval_operands_loop(frame, operands, 0, mlc::Array<value::VmValue>{});
}

} // namespace mir_eval
