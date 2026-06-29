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

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept;

bool vm_values_equal(value::VmValue left, value::VmValue right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary_i32(mlc::String operation, int left, int right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept;

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept;

mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept;

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept{return std::visit(overloaded{
  [&](const MirOperandLocal& miroperandlocal) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [local_id] = miroperandlocal; return frame::vm_locals_load(frame.locals, local_id); },
  [&](const MirOperandConstInt& miroperandconstint) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = miroperandconstint; return ast::Ok<value::VmValue>(value::VmI32(value)); },
  [&](const MirOperandConstBool& miroperandconstbool) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = miroperandconstbool; return ast::Ok<value::VmValue>(value::VmBool(value)); },
  [&](const MirOperandConstStr& miroperandconststr) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [value] = miroperandconststr; return ast::Ok<value::VmValue>(value::VmString(value)); },
  [&](const MirOperandUnit& miroperandunit) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Ok<value::VmValue>(value::vm_value_unit()); }
}, operand);}

bool vm_values_equal(value::VmValue left, value::VmValue right) noexcept{return std::visit(overloaded{
  [&](const VmI32& vmi32) -> bool { auto [left_number] = vmi32; return [&]() { if (std::holds_alternative<value::VmI32>(right)) { auto _v_vmi32 = std::get<value::VmI32>(right); auto [right_number] = _v_vmi32; return left_number == right_number; } return false; }(); },
  [&](const VmBool& vmbool) -> bool { auto [left_flag] = vmbool; return [&]() { if (std::holds_alternative<value::VmBool>(right)) { auto _v_vmbool = std::get<value::VmBool>(right); auto [right_flag] = _v_vmbool; return left_flag == right_flag; } return false; }(); },
  [&](const VmString& vmstring) -> bool { auto [left_text] = vmstring; return [&]() { if (std::holds_alternative<value::VmString>(right)) { auto _v_vmstring = std::get<value::VmString>(right); auto [right_text] = _v_vmstring; return left_text == right_text; } return false; }(); },
  [&](const VmArray& vmarray) -> bool { auto [left_array] = vmarray; return [&]() { if (std::holds_alternative<value::VmArray>(right)) { auto _v_vmarray = std::get<value::VmArray>(right); auto [right_array] = _v_vmarray; return left_array.integer_elements.size() == right_array.integer_elements.size(); } return false; }(); },
  [&](const VmMap& vmmap) -> bool { auto [_w0] = vmmap; return false; },
  [&](const VmVariant& vmvariant) -> bool { auto [_w0] = vmvariant; return false; },
  [&](const VmUnit& vmunit) -> bool { return [&]() { if (std::holds_alternative<value::VmUnit>(right)) {  return true; } return false; }(); }
}, left);}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary_i32(mlc::String operation, int left, int right) noexcept{return operation == mlc::String("+") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(left + right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("-") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(left - right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("*") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(left * right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("/") && right != 0 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(left / right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("/") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: division by zero")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("%") && right != 0 ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmI32(left % right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("%") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: modulo by zero")})) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String(">") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left > right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("<") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left < right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String(">=") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left >= right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("<=") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left <= right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("==") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left == right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("!=") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(left != right))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unknown int binary ") + operation}))))))))))))));}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept{return operation == mlc::String("&&") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(value::vm_value_is_truthy(left) && value::vm_value_is_truthy(right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("||") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(value::vm_value_is_truthy(left) || value::vm_value_is_truthy(right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("==") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(vm_values_equal(left, right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("!=") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(!vm_values_equal(left, right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("+") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(left)) { auto _v_vmstring = std::get<value::VmString>(left); auto [left_text] = _v_vmstring; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(right)) { auto _v_vmstring = std::get<value::VmString>(right); auto [right_text] = _v_vmstring; return ast::Ok<value::VmValue>(value::VmString(left_text + right_text)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: string concat requires string operand")}); }(); } if (std::holds_alternative<value::VmI32>(left)) { auto _v_vmi32 = std::get<value::VmI32>(left); auto [left_number] = _v_vmi32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(right)) { auto _v_vmi32 = std::get<value::VmI32>(right); auto [right_number] = _v_vmi32; return vm_eval_binary_i32(operation, left_number, right_number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported binary + operand")}); }()) : ast::Result<value::VmValue, mlc::Array<mlc::String>>([&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(left)) { auto _v_vmi32 = std::get<value::VmI32>(left); auto [left_number] = _v_vmi32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(right)) { auto _v_vmi32 = std::get<value::VmI32>(right); auto [right_number] = _v_vmi32; return vm_eval_binary_i32(operation, left_number, right_number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand")}); }(); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported binary ") + operation}); }())))));}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept{return std::visit(overloaded{
  [&](const MirRvalueUse& mirrvalueuse) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [operand] = mirrvalueuse; return vm_eval_operand(frame, operand); },
  [&](const MirRvalueBinary& mirrvaluebinary) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [operation, left_operand, right_operand] = mirrvaluebinary; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [left_value] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [right_value] = ok; return vm_eval_binary(operation, left_value, right_value); }
}, vm_eval_operand(frame, right_operand)); }
}, vm_eval_operand(frame, left_operand)); }
}, rvalue);}

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept{return index >= operands.size() ? ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>>(ast::Ok<mlc::Array<value::VmValue>>(values)) : ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [value] = ok; return vm_eval_operands_loop(frame, operands, index + 1, vm_values_push(values, value)); }
}, vm_eval_operand(frame, operands[index])));}

mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept{
mlc::Array<value::VmValue> next_values = values;
next_values.push_back(value);
return next_values;
}

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept{return vm_eval_operands_loop(frame, operands, 0, {});}

} // namespace mir_eval
