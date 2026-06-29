#include "execute.hpp"

#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "mir_eval.hpp"
#include "runtime.hpp"
#include "outcome.hpp"

namespace execute {

using namespace ast;
using namespace mir_types;
using namespace value;
using namespace frame;
using namespace mir_eval;
using namespace runtime;
using namespace outcome;
using namespace ast_tokens;

outcome::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept;

outcome::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept;

ast::Result<mir_types::MirBlock, mlc::Array<mlc::String>> vm_find_block(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept;

outcome::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept;

outcome::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept;

outcome::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept;

mlc::Array<frame::VmFrame> vm_drop_last_frame(mlc::Array<frame::VmFrame> frames) noexcept;

outcome::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept;

outcome::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept{return vm_run_frames(program, mlc::Array<frame::VmFrame>{frame::vm_frame_new(function, mir_types::mir_block_id(0), runtime::vm_bind_call_arguments(function, arguments))}, trace_enabled);}

outcome::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept{
if (frames.size() == 0){
{
return outcome::VmRunFailed(mlc::Array<mlc::String>{mlc::String("vm: empty frame stack")});
}
}
int frame_index = frames.size() - 1;
frame::VmFrame frame = frame::vm_frame_at(frames, frame_index);
return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<mir_types::MirBlock>& ok) -> outcome::VmRunOutcome { auto [block] = ok; return frame.statement_index < block.stmts.size() ? outcome::VmRunOutcome(std::visit(overloaded{
  [&](const VmRunContinue& vmruncontinue) -> outcome::VmRunOutcome { auto [next_frames] = vmruncontinue; return vm_run_frames(program, next_frames, trace_enabled); },
  [&](const VmRunReturn& vmrunreturn) -> outcome::VmRunOutcome { auto [value] = vmrunreturn; return outcome::VmRunReturn(value); },
  [&](const VmRunFailed& vmrunfailed) -> outcome::VmRunOutcome { auto [errors] = vmrunfailed; return outcome::VmRunFailed(errors); }
}, vm_execute_statement(program, frames, frame_index, block.stmts[frame.statement_index], trace_enabled))) : outcome::VmRunOutcome(std::visit(overloaded{
  [&](const VmRunContinue& vmruncontinue) -> outcome::VmRunOutcome { auto [next_frames] = vmruncontinue; return vm_run_frames(program, next_frames, trace_enabled); },
  [&](const VmRunReturn& vmrunreturn) -> outcome::VmRunOutcome { auto [value] = vmrunreturn; return outcome::VmRunReturn(value); },
  [&](const VmRunFailed& vmrunfailed) -> outcome::VmRunOutcome { auto [errors] = vmrunfailed; return outcome::VmRunFailed(errors); }
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

outcome::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept{
frame::VmFrame caller_frame = frame::VmFrame{frame::vm_frame_at(frames, frame_index).function, frame::vm_frame_at(frames, frame_index).block_id, frame::vm_frame_at(frames, frame_index).statement_index, frame::vm_frame_at(frames, frame_index).locals, local_id.index};
mlc::Array<frame::VmFrame> next_frames = frame::vm_frame_update(frames, frame_index, caller_frame);
next_frames.push_back(frame::vm_frame_new(callee, mir_types::mir_block_id(0), runtime::vm_bind_call_arguments(callee, argument_values)));
if (trace_enabled){
{
mlc::io::println(mlc::String("vm call ") + callee_name);
}
}
return outcome::VmRunContinue(next_frames);
}

outcome::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept{
frame::VmFrame frame = frame::vm_frame_at(frames, frame_index);
return std::visit(overloaded{
  [&](const MirAssign& mirassign) -> outcome::VmRunOutcome { auto [local_id, rvalue] = mirassign; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue(frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)); }
}, mir_eval::vm_eval_rvalue(frame, rvalue)); },
  [&](const MirCallAssign& mircallassign) -> outcome::VmRunOutcome { auto [local_id, callee_name, arguments] = mircallassign; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<mlc::Array<value::VmValue>>& ok) -> outcome::VmRunOutcome { auto [argument_values] = ok; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<runtime::VmCallDispatch>& ok) -> outcome::VmRunOutcome { auto [dispatch] = ok; return [&]() -> outcome::VmRunOutcome { if (std::holds_alternative<runtime::VmCallUser>(dispatch)) { auto _v_vmcalluser = std::get<runtime::VmCallUser>(dispatch); auto [callee] = _v_vmcalluser; return vm_execute_user_call(program, frames, frame_index, local_id, callee, argument_values, callee_name, trace_enabled); } if (std::holds_alternative<runtime::VmCallNative>(dispatch)) { auto _v_vmcallnative = std::get<runtime::VmCallNative>(dispatch); auto [_w0] = _v_vmcallnative; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue(frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)); }
}, runtime::vm_dispatch_call(dispatch, callee_name, argument_values)); } else if (std::holds_alternative<runtime::VmCallVariantCtor>(dispatch)) { auto _v_vmcallvariantctor = std::get<runtime::VmCallVariantCtor>(dispatch); return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue(frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)); }
}, runtime::vm_dispatch_call(dispatch, callee_name, argument_values)); } }(); }
}, runtime::vm_resolve_call(program, callee_name)); }
}, mir_eval::vm_eval_operands(frame, arguments)); }
}, statement);
}

outcome::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept{
frame::VmFrame frame = frame::vm_frame_at(frames, frame_index);
return std::visit(overloaded{
  [&](const MirReturn& mirreturn) -> outcome::VmRunOutcome { auto [operand] = mirreturn; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return frames.size() == 1 ? outcome::VmRunOutcome(outcome::VmRunReturn(value)) : outcome::VmRunOutcome(vm_pop_return(program, frames, frame_index, value, trace_enabled)); }
}, mir_eval::vm_eval_operand(frame, operand)); },
  [&](const MirJump& mirjump) -> outcome::VmRunOutcome { auto [block_id] = mirjump; return outcome::VmRunContinue(frame::vm_frame_set_block(frames, frame_index, block_id)); },
  [&](const MirCondJump& mircondjump) -> outcome::VmRunOutcome { auto [operand, then_block, else_block] = mircondjump; return std::visit(overloaded{
  [&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed(errors); },
  [&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return value::vm_value_is_truthy(value) ? outcome::VmRunOutcome(outcome::VmRunContinue(frame::vm_frame_set_block(frames, frame_index, then_block))) : outcome::VmRunOutcome(outcome::VmRunContinue(frame::vm_frame_set_block(frames, frame_index, else_block))); }
}, mir_eval::vm_eval_operand(frame, operand)); },
  [&](const MirUnreachable& mirunreachable) -> outcome::VmRunOutcome { return outcome::VmRunFailed(mlc::Array<mlc::String>{mlc::String("vm: unreachable block")}); }
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

outcome::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept{
mlc::Array<frame::VmFrame> caller_frames = vm_drop_last_frame(frames);
if (caller_frames.size() == 0){
{
return outcome::VmRunReturn(return_value);
}
}
int caller_index = caller_frames.size() - 1;
frame::VmFrame caller_frame = frame::vm_frame_at(caller_frames, caller_index);
if (caller_frame.pending_call_local >= 0){
{
caller_frame = frame::VmFrame{caller_frame.function, caller_frame.block_id, caller_frame.statement_index, frame::vm_locals_store(caller_frame.locals, mir_types::LocalId{caller_frame.pending_call_local}, return_value), -1};
caller_frames = frame::vm_frame_update(caller_frames, caller_index, caller_frame);
}
}
return vm_run_frames(program, frame::vm_frame_advance_statement(caller_frames, caller_index), trace_enabled);
}

} // namespace execute
