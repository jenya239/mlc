#define main mlc_user_main
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

outcome::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept{
  return vm_run_frames(program, mlc::Array<frame::VmFrame>{frame::vm_frame_new(function, mir_types::mir_block_id(0), runtime::vm_bind_call_arguments(function, arguments))}, trace_enabled);
}
outcome::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept{
  if ((frames.length() == 0))   {
    return outcome::VmRunFailed{mlc::Array<mlc::String>{mlc::String("vm: empty frame stack", 21)}};
  }
  auto frame_index = (frames.length() - 1);
  auto frame = frame::vm_frame_at(frames, frame_index);
  return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<mir_types::MirBlock>& ok) -> outcome::VmRunOutcome { auto [block] = ok; return ((frame.statement_index < block.stmts.length()) ? (std::visit(overloaded{[&](const outcome::VmRunContinue& vmRunContinue) -> outcome::VmRunOutcome { auto [next_frames] = vmRunContinue; return vm_run_frames(program, next_frames, trace_enabled); },
[&](const outcome::VmRunReturn& vmRunReturn) -> outcome::VmRunOutcome { auto [value] = vmRunReturn; return outcome::VmRunReturn{value}; },
[&](const outcome::VmRunFailed& vmRunFailed) -> outcome::VmRunOutcome { auto [errors] = vmRunFailed; return outcome::VmRunFailed{errors}; }
}, vm_execute_statement(program, frames, frame_index, block.stmts[frame.statement_index], trace_enabled))) : (std::visit(overloaded{[&](const outcome::VmRunContinue& vmRunContinue) -> outcome::VmRunOutcome { auto [next_frames] = vmRunContinue; return vm_run_frames(program, next_frames, trace_enabled); },
[&](const outcome::VmRunReturn& vmRunReturn) -> outcome::VmRunOutcome { auto [value] = vmRunReturn; return outcome::VmRunReturn{value}; },
[&](const outcome::VmRunFailed& vmRunFailed) -> outcome::VmRunOutcome { auto [errors] = vmRunFailed; return outcome::VmRunFailed{errors}; }
}, vm_execute_terminator(program, frames, frame_index, block.term, trace_enabled)))); }
}, vm_find_block(frame.function, frame.block_id));
}
ast::Result<mir_types::MirBlock, mlc::Array<mlc::String>> vm_find_block(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept{
  auto index = 0;
  while ((index < function.blocks.length()))   {
    auto block = function.blocks[index];
    if ((block.id.index == block_id.index))     {
      return ast::Ok<mir_types::MirBlock>{block};
    }
    (index = (index + 1));
  }
  return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: block ", 10) + mlc::to_string(block_id.index)) + mlc::String(" not found", 10))}};
}
outcome::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept{
  auto caller_frame = frame::VmFrame{frame::vm_frame_at(frames, frame_index).function, frame::vm_frame_at(frames, frame_index).block_id, frame::vm_frame_at(frames, frame_index).statement_index, frame::vm_frame_at(frames, frame_index).locals, local_id.index};
  auto next_frames = frame::vm_frame_update(frames, frame_index, caller_frame);
  next_frames.push_back(frame::vm_frame_new(callee, mir_types::mir_block_id(0), runtime::vm_bind_call_arguments(callee, argument_values)));
  if (trace_enabled)   {
    mlc::io::println(((mlc::String("vm call ", 8) + mlc::to_string(callee_name)) + mlc::String("", 0)));
  }
  return outcome::VmRunContinue{next_frames};
}
outcome::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept{
  auto frame = frame::vm_frame_at(frames, frame_index);
  return std::visit(overloaded{[&](const mir_types::MirAssign& mirAssign) -> outcome::VmRunOutcome { auto [local_id, rvalue] = mirAssign; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue{frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)}; }
}, mir_eval::vm_eval_rvalue(frame, rvalue)); },
[&](const mir_types::MirCallAssign& mirCallAssign) -> outcome::VmRunOutcome { auto [local_id, callee_name, arguments] = mirCallAssign; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<mlc::Array<value::VmValue>>& ok) -> outcome::VmRunOutcome { auto [argument_values] = ok; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<runtime::VmCallDispatch>& ok) -> outcome::VmRunOutcome { auto [dispatch] = ok; return std::visit(overloaded{[&](const runtime::VmCallUser& vmCallUser) -> outcome::VmRunOutcome { auto [callee] = vmCallUser; return vm_execute_user_call(program, frames, frame_index, local_id, callee, argument_values, callee_name, trace_enabled); },
[&](const runtime::VmCallNative& vmCallNative) -> outcome::VmRunOutcome { auto [__0] = vmCallNative; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue{frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)}; }
}, runtime::vm_dispatch_call(dispatch, callee_name, argument_values)); },
[&](const runtime::VmCallVariantCtor& vmCallVariantCtor) -> outcome::VmRunOutcome { auto [__0] = vmCallVariantCtor; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return outcome::VmRunContinue{frame::vm_frame_advance_statement(frame::vm_frame_set_locals(frames, frame_index, frame::vm_locals_store(frame.locals, local_id, value)), frame_index)}; }
}, runtime::vm_dispatch_call(dispatch, callee_name, argument_values)); }
}, dispatch); }
}, runtime::vm_resolve_call(program, callee_name)); }
}, mir_eval::vm_eval_operands(frame, arguments)); }
}, statement);
}
outcome::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept{
  auto frame = frame::vm_frame_at(frames, frame_index);
  return std::visit(overloaded{[&](const mir_types::MirReturn& mirReturn) -> outcome::VmRunOutcome { auto [operand] = mirReturn; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return ((frames.length() == 1) ? (outcome::VmRunReturn{value}) : (vm_pop_return(program, frames, frame_index, value, trace_enabled))); }
}, mir_eval::vm_eval_operand(frame, operand)); },
[&](const mir_types::MirJump& mirJump) -> outcome::VmRunOutcome { auto [block_id] = mirJump; return outcome::VmRunContinue{frame::vm_frame_set_block(frames, frame_index, block_id)}; },
[&](const mir_types::MirCondJump& mirCondJump) -> outcome::VmRunOutcome { auto [operand, then_block, else_block] = mirCondJump; return std::visit(overloaded{[&](const ast::Err<mlc::Array<mlc::String>>& err) -> outcome::VmRunOutcome { auto [errors] = err; return outcome::VmRunFailed{errors}; },
[&](const ast::Ok<value::VmValue>& ok) -> outcome::VmRunOutcome { auto [value] = ok; return (value::vm_value_is_truthy(value) ? (outcome::VmRunContinue{frame::vm_frame_set_block(frames, frame_index, then_block)}) : (outcome::VmRunContinue{frame::vm_frame_set_block(frames, frame_index, else_block)})); }
}, mir_eval::vm_eval_operand(frame, operand)); },
[&](const mir_types::MirUnreachable& mirUnreachable) -> outcome::VmRunOutcome { return outcome::VmRunFailed{mlc::Array<mlc::String>{mlc::String("vm: unreachable block", 21)}}; }
}, terminator);
}
mlc::Array<frame::VmFrame> vm_drop_last_frame(mlc::Array<frame::VmFrame> frames) noexcept{
  auto next_frames = mlc::Array<frame::VmFrame>{};
  auto index = 0;
  while ((index < (frames.length() - 1)))   {
    next_frames.push_back(frames[index]);
    (index = (index + 1));
  }
  return next_frames;
}
outcome::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept{
  auto caller_frames = vm_drop_last_frame(frames);
  if ((caller_frames.length() == 0))   {
    return outcome::VmRunReturn{return_value};
  }
  auto caller_index = (caller_frames.length() - 1);
  auto caller_frame = frame::vm_frame_at(caller_frames, caller_index);
  if ((caller_frame.pending_call_local >= 0))   {
    (caller_frame = frame::VmFrame{caller_frame.function, caller_frame.block_id, caller_frame.statement_index, frame::vm_locals_store(caller_frame.locals, mir_types::LocalId{caller_frame.pending_call_local}, return_value), (-1)});
    (caller_frames = frame::vm_frame_update(caller_frames, caller_index, caller_frame));
  }
  return vm_run_frames(program, frame::vm_frame_advance_statement(caller_frames, caller_index), trace_enabled);
}

} // namespace execute
