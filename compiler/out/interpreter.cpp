#include "interpreter.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "lower_program.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "native.hpp"

namespace interpreter {

using namespace ast;
using namespace semantic_ir;
using namespace lower_program;
using namespace mir_types;
using namespace value;
using namespace frame;
using namespace native;
using namespace ast_tokens;

struct VmRunContinue {mlc::Array<frame::VmFrame> field0;};struct VmRunReturn {value::VmValue field0;};struct VmRunFailed {mlc::Array<mlc::String> field0;};using VmRunOutcome = std::variant<VmRunContinue, VmRunReturn, VmRunFailed>;

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept;

ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept;

ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept;

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept;

interpreter::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept;

interpreter::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept;

ast::Result<mir_types::MirBlock, mlc::Array<mlc::String>> vm_find_block(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept;

frame::VmFrame vm_frame_at(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_update(mlc::Array<frame::VmFrame> frames, int frame_index, frame::VmFrame frame) noexcept;

mlc::Array<frame::VmFrame> vm_frame_advance_statement(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_block(mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::BlockId block_id) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_locals(mlc::Array<frame::VmFrame> frames, int frame_index, mlc::Array<value::VmValue> locals) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_operand(frame::VmFrame frame, mir_types::MirOperand operand) noexcept;

bool vm_values_equal(value::VmValue left, value::VmValue right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary_i32(mlc::String operation, int left, int right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_rvalue(frame::VmFrame frame, mir_types::MirRvalue rvalue) noexcept;

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept;

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept;

mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept;

interpreter::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept;

interpreter::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept;

bool vm_is_variant_ctor_name(mlc::String name) noexcept;

bool vm_is_native_callee(mlc::String name) noexcept;

mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction function, mlc::Array<value::VmValue> arguments) noexcept;

interpreter::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept;

mlc::Array<frame::VmFrame> vm_drop_last_frame(mlc::Array<frame::VmFrame> frames) noexcept;

interpreter::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept;

ast::Result<int, mlc::Array<mlc::String>> interpret_mir_program(mir_types::MirProgram program, bool trace_enabled) noexcept{
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [function] = ok; return std::visit(overloaded{
  [&](const VmRunReturn& vmrunreturn) -> ast::Result<int, mlc::Array<mlc::String>> { auto [value] = vmrunreturn; return std::visit(overloaded{
  [&](const ast::Ok<int>& ok) -> ast::Result<int, mlc::Array<mlc::String>> { auto [code] = ok; return ast::Ok<int>(code); },
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<int, mlc::Array<mlc::String>> { auto [_w0] = err; return ast::Ok<int>(0); }
}, value::vm_value_as_i32(value)); },
  [&](const VmRunFailed& vmrunfailed) -> ast::Result<int, mlc::Array<mlc::String>> { auto [errors] = vmrunfailed; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const VmRunContinue& vmruncontinue) -> ast::Result<int, mlc::Array<mlc::String>> { auto [_w0] = vmruncontinue; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: main returned without value")}); }
}, vm_run_function(program, function, {}, trace_enabled)); }
}, vm_find_function(program, mlc::String("main")));
}

ast::Result<int, mlc::Array<mlc::String>> run_mir_program_from_semantic_items(mlc::Array<semantic_ir::SemanticLoadItem> items, bool trace_enabled) noexcept{return interpret_mir_program(lower_program::build_mir_program_from_semantic_items(items), trace_enabled);}

ast::Result<value::VmValue, mlc::String> interpret_mir_function(mir_types::MirFunction function) noexcept{
mir_types::MirProgram program = mir_types::MirProgram{mlc::Array<mir_types::MirModule>{mir_types::MirModule{mlc::Array<mir_types::MirFunction>{function}}}};
return std::visit(overloaded{
  [&](const VmRunReturn& vmrunreturn) -> ast::Result<value::VmValue, mlc::String> { auto [value] = vmrunreturn; return ast::Ok<value::VmValue>(value); },
  [&](const VmRunFailed& vmrunfailed) -> ast::Result<value::VmValue, mlc::String> { auto [errors] = vmrunfailed; return errors.size() > 0 ? ast::Result<value::VmValue, mlc::String>(ast::Err<mlc::String>(errors[0])) : ast::Result<value::VmValue, mlc::String>(ast::Err<mlc::String>(mlc::String("vm: interpreter failed"))); },
  [&](const VmRunContinue& vmruncontinue) -> ast::Result<value::VmValue, mlc::String> { auto [_w0] = vmruncontinue; return ast::Err<mlc::String>(mlc::String("vm: function did not return")); }
}, vm_run_function(program, function, {}, false));
}

ast::Result<mir_types::MirFunction, mlc::Array<mlc::String>> vm_find_function(mir_types::MirProgram program, mlc::String name) noexcept{
int module_index = 0;
while (module_index < program.modules.size()){
{
mir_types::MirModule mir_module = program.modules[module_index];
int function_index = 0;
while (function_index < mir_module.functions.size()){
{
mir_types::MirFunction function = mir_module.functions[function_index];
if (function.name == name){
{
return ast::Ok<mir_types::MirFunction>(function);
}
}
function_index = function_index + 1;
}
}
module_index = module_index + 1;
}
}
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: function ") + name + mlc::String(" not found")});
}

interpreter::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept{
mlc::Array<value::VmValue> locals = {};
int index = 0;
while (index < arguments.size()){
{
locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, arguments[index]);
index = index + 1;
}
}
return vm_run_frames(program, mlc::Array<frame::VmFrame>{frame::vm_frame_new(function, mir_types::mir_block_id(0), locals)}, trace_enabled);
}

interpreter::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept{
if (frames.size() == 0){
{
return interpreter::VmRunFailed(mlc::Array<mlc::String>{mlc::String("vm: empty frame stack")});
}
}
int frame_index = frames.size() - 1;
frame::VmFrame frame = frames[frame_index];
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<mir_types::MirBlock>& ok) -> interpreter::VmRunOutcome { auto [block] = ok; return frame.statement_index < block.stmts.size() ? interpreter::VmRunOutcome(std::visit(overloaded{
  [&](const VmRunContinue& vmruncontinue) -> interpreter::VmRunOutcome { auto [next_frames] = vmruncontinue; return vm_run_frames(program, next_frames, trace_enabled); },
  [&](const VmRunReturn& vmrunreturn) -> interpreter::VmRunOutcome { auto [value] = vmrunreturn; return interpreter::VmRunReturn(value); },
  [&](const VmRunFailed& vmrunfailed) -> interpreter::VmRunOutcome { auto [errors] = vmrunfailed; return interpreter::VmRunFailed(errors); }
}, vm_execute_statement(program, frames, frame_index, block.stmts[frame.statement_index], trace_enabled))) : interpreter::VmRunOutcome(std::visit(overloaded{
  [&](const VmRunContinue& vmruncontinue) -> interpreter::VmRunOutcome { auto [next_frames] = vmruncontinue; return vm_run_frames(program, next_frames, trace_enabled); },
  [&](const VmRunReturn& vmrunreturn) -> interpreter::VmRunOutcome { auto [value] = vmrunreturn; return interpreter::VmRunReturn(value); },
  [&](const VmRunFailed& vmrunfailed) -> interpreter::VmRunOutcome { auto [errors] = vmrunfailed; return interpreter::VmRunFailed(errors); }
}, vm_execute_terminator(program, frames, frame_index, block.term, trace_enabled))); }
}, vm_find_block(frame.function, frame.block_id));
}

ast::Result<mir_types::MirBlock, mlc::Array<mlc::String>> vm_find_block(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept{
int index = 0;
while (index < function.blocks.size()){
{
mir_types::MirBlock block = function.blocks[index];
if (block.id.index == block_id.index){
{
return ast::Ok<mir_types::MirBlock>(block);
}
}
index = index + 1;
}
}
return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: block ") + mlc::to_string(block_id.index) + mlc::String(" not found")});
}

frame::VmFrame vm_frame_at(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept{return frames[frame_index];}

mlc::Array<frame::VmFrame> vm_frame_update(mlc::Array<frame::VmFrame> frames, int frame_index, frame::VmFrame frame) noexcept{
mlc::Array<frame::VmFrame> rebuilt = {};
int index = 0;
while (index < frames.size()){
{
if (index == frame_index){
{
rebuilt.push_back(frame);
}
} else {
{
rebuilt.push_back(frames[index]);
}
}
index = index + 1;
}
}
return rebuilt;
}

mlc::Array<frame::VmFrame> vm_frame_advance_statement(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept{
frame::VmFrame frame = vm_frame_at(frames, frame_index);
return vm_frame_update(frames, frame_index, frame::VmFrame{frame.function, frame.block_id, frame.statement_index + 1, frame.locals, frame.pending_call_local});
}

mlc::Array<frame::VmFrame> vm_frame_set_block(mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::BlockId block_id) noexcept{
frame::VmFrame frame = vm_frame_at(frames, frame_index);
return vm_frame_update(frames, frame_index, frame::VmFrame{frame.function, block_id, 0, frame.locals, frame.pending_call_local});
}

mlc::Array<frame::VmFrame> vm_frame_set_locals(mlc::Array<frame::VmFrame> frames, int frame_index, mlc::Array<value::VmValue> locals) noexcept{
frame::VmFrame frame = vm_frame_at(frames, frame_index);
return vm_frame_update(frames, frame_index, frame::VmFrame{frame.function, frame.block_id, frame.statement_index, locals, frame.pending_call_local});
}

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

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_eval_binary(mlc::String operation, value::VmValue left, value::VmValue right) noexcept{return operation == mlc::String("&&") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(value::vm_value_is_truthy(left) && value::vm_value_is_truthy(right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("||") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(value::vm_value_is_truthy(left) || value::vm_value_is_truthy(right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("==") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(vm_values_equal(left, right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("!=") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(value::VmBool(!vm_values_equal(left, right)))) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(operation == mlc::String("+") ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const VmString& vmstring) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [left_text] = vmstring; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmString>(right)) { auto _v_vmstring = std::get<value::VmString>(right); auto [right_text] = _v_vmstring; return ast::Ok<value::VmValue>(value::VmString(left_text + right_text)); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: string concat requires string operand")}); }(); },
  [&](const VmI32& vmi32) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [left_number] = vmi32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(right)) { auto _v_vmi32 = std::get<value::VmI32>(right); auto [right_number] = _v_vmi32; return vm_eval_binary_i32(operation, left_number, right_number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand")}); }(); },
  [&](const VmBool& vmbool) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmbool; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported bool binary")}); },
  [&](const VmArray& vmarray) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmarray; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported array binary")}); },
  [&](const VmMap& vmmap) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmmap; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported map binary")}); },
  [&](const VmVariant& vmvariant) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmvariant; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported variant binary")}); },
  [&](const VmUnit& vmunit) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported unit binary")}); }
}, left)) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const VmI32& vmi32) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [left_number] = vmi32; return [&]() -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { if (std::holds_alternative<value::VmI32>(right)) { auto _v_vmi32 = std::get<value::VmI32>(right); auto [right_number] = _v_vmi32; return vm_eval_binary_i32(operation, left_number, right_number); } return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: binary i32 operation on non-i32 operand")}); }(); },
  [&](const VmBool& vmbool) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmbool; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported bool binary")}); },
  [&](const VmString& vmstring) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmstring; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported string binary")}); },
  [&](const VmArray& vmarray) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmarray; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported array binary")}); },
  [&](const VmMap& vmmap) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmmap; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported map binary")}); },
  [&](const VmVariant& vmvariant) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { auto [_w0] = vmvariant; return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported variant binary")}); },
  [&](const VmUnit& vmunit) -> ast::Result<value::VmValue, mlc::Array<mlc::String>> { return ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unsupported unit binary")}); }
}, left))))));}

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

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands) noexcept{return vm_eval_operands_loop(frame, operands, 0, {});}

ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> vm_eval_operands_loop(frame::VmFrame frame, mlc::Array<mir_types::MirOperand> operands, int index, mlc::Array<value::VmValue> values) noexcept{return index >= operands.size() ? ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>>(ast::Ok<mlc::Array<value::VmValue>>(values)) : ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>>(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [errors] = err; return ast::Err<mlc::Array<mlc::String>>(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> ast::Result<mlc::Array<value::VmValue>, mlc::Array<mlc::String>> { auto [value] = ok; return vm_eval_operands_loop(frame, operands, index + 1, vm_values_push(values, value)); }
}, vm_eval_operand(frame, operands[index])));}

mlc::Array<value::VmValue> vm_values_push(mlc::Array<value::VmValue> values, value::VmValue value) noexcept{
mlc::Array<value::VmValue> next_values = values;
next_values.push_back(value);
return next_values;
}

interpreter::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept{
frame::VmFrame caller_frame = frame::VmFrame{vm_frame_at(frames, frame_index).function, vm_frame_at(frames, frame_index).block_id, vm_frame_at(frames, frame_index).statement_index, vm_frame_at(frames, frame_index).locals, local_id.index};
mlc::Array<frame::VmFrame> next_frames = vm_frame_update(frames, frame_index, caller_frame);
next_frames.push_back(frame::vm_frame_new(callee, mir_types::mir_block_id(0), vm_bind_call_arguments(callee, argument_values)));
if (trace_enabled){
{
mlc::io::println(mlc::String("vm call ") + callee_name);
}
}
return interpreter::VmRunContinue(next_frames);
}

interpreter::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept{
frame::VmFrame frame = vm_frame_at(frames, frame_index);
return std::visit(overloaded{
  [&](const MirAssign& mirassign) -> interpreter::VmRunOutcome { auto [local_id, rvalue] = mirassign; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> interpreter::VmRunOutcome { auto [value] = ok; return interpreter::VmRunContinue(vm_frame_advance_statement(vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)); }
}, vm_eval_rvalue(frame, rvalue)); },
  [&](const MirCallAssign& mircallassign) -> interpreter::VmRunOutcome { auto [local_id, callee_name, arguments] = mircallassign; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<mlc::Array<value::VmValue>>& ok) -> interpreter::VmRunOutcome { auto [argument_values] = ok; return vm_is_native_callee(callee_name) ? interpreter::VmRunOutcome(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> interpreter::VmRunOutcome { auto [value] = ok; return interpreter::VmRunContinue(vm_frame_advance_statement(vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)); }
}, native::vm_native_call(callee_name, argument_values))) : interpreter::VmRunOutcome(vm_is_variant_ctor_name(callee_name) ? interpreter::VmRunOutcome(interpreter::VmRunContinue(vm_frame_advance_statement(vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, native::vm_variant_ctor(callee_name, argument_values))), frame_index))) : interpreter::VmRunOutcome(std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<mir_types::MirFunction>& ok) -> interpreter::VmRunOutcome { auto [callee] = ok; return vm_execute_user_call(program, frames, frame_index, local_id, callee, argument_values, callee_name, trace_enabled); }
}, vm_find_function(program, callee_name)))); }
}, vm_eval_operands(frame, arguments)); }
}, statement);
}

bool vm_is_variant_ctor_name(mlc::String name) noexcept{return name.length() > 0 && name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}

bool vm_is_native_callee(mlc::String name) noexcept{return name == mlc::String("println") || name == mlc::String("__mir_array_empty") || name == mlc::String("__mir_array_push") || name == mlc::String("__mir_array_length") || name == mlc::String("__mir_array_get") || name == mlc::String("__mir_map_empty") || name == mlc::String("__mir_map_set") || name == mlc::String("__mir_map_get") || name == mlc::String("__mir_map_length") || name == mlc::String("__mir_string_length") || name == mlc::String("__mir_length") || name == mlc::String("__mir_variant_is") || name == mlc::String("__mir_variant_field");}

mlc::Array<value::VmValue> vm_bind_call_arguments(mir_types::MirFunction function, mlc::Array<value::VmValue> arguments) noexcept{
mlc::Array<value::VmValue> locals = {};
int index = 0;
while (index < arguments.size()){
{
locals = frame::vm_locals_store(locals, mir_types::LocalId{index}, arguments[index]);
index = index + 1;
}
}
return locals;
}

interpreter::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept{
frame::VmFrame frame = vm_frame_at(frames, frame_index);
return std::visit(overloaded{
  [&](const MirReturn& mirreturn) -> interpreter::VmRunOutcome { auto [operand] = mirreturn; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> interpreter::VmRunOutcome { auto [value] = ok; return frames.size() == 1 ? interpreter::VmRunOutcome(interpreter::VmRunReturn(value)) : interpreter::VmRunOutcome(vm_pop_return(program, frames, frame_index, value, trace_enabled)); }
}, vm_eval_operand(frame, operand)); },
  [&](const MirJump& mirjump) -> interpreter::VmRunOutcome { auto [block_id] = mirjump; return interpreter::VmRunContinue(vm_frame_set_block(frames, frame_index, block_id)); },
  [&](const MirCondJump& mircondjump) -> interpreter::VmRunOutcome { auto [operand, then_block, else_block] = mircondjump; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> interpreter::VmRunOutcome { auto [errors] = err; return interpreter::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> interpreter::VmRunOutcome { auto [value] = ok; return value::vm_value_is_truthy(value) ? interpreter::VmRunOutcome(interpreter::VmRunContinue(vm_frame_set_block(frames, frame_index, then_block))) : interpreter::VmRunOutcome(interpreter::VmRunContinue(vm_frame_set_block(frames, frame_index, else_block))); }
}, vm_eval_operand(frame, operand)); },
  [&](const MirUnreachable& mirunreachable) -> interpreter::VmRunOutcome { return interpreter::VmRunFailed(mlc::Array<mlc::String>{mlc::String("vm: unreachable block")}); }
}, terminator);
}

mlc::Array<frame::VmFrame> vm_drop_last_frame(mlc::Array<frame::VmFrame> frames) noexcept{
mlc::Array<frame::VmFrame> next_frames = {};
int index = 0;
while (index < frames.size() - 1){
{
next_frames.push_back(frames[index]);
index = index + 1;
}
}
return next_frames;
}

interpreter::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept{
mlc::Array<frame::VmFrame> caller_frames = vm_drop_last_frame(frames);
if (caller_frames.size() == 0){
{
return interpreter::VmRunReturn(return_value);
}
}
int caller_index = caller_frames.size() - 1;
frame::VmFrame caller_frame = vm_frame_at(caller_frames, caller_index);
if (caller_frame.pending_call_local >= 0){
{
caller_frame = frame::VmFrame{caller_frame.function, caller_frame.block_id, caller_frame.statement_index, frame::vm_locals_store(caller_frame.locals, mir_types::LocalId{caller_frame.pending_call_local}, return_value), -1};
caller_frames = vm_frame_update(caller_frames, caller_index, caller_frame);
}
}
return vm_run_frames(program, vm_frame_advance_statement(caller_frames, caller_index), trace_enabled);
}

} // namespace interpreter
