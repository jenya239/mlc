#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "mir_types.hpp"
#include "value.hpp"
#include "frame.hpp"
#include "mir_eval.hpp"
#include "runtime.hpp"
#include "outcome.hpp"
namespace execute {

outcome::VmRunOutcome vm_run_function(mir_types::MirProgram program, mir_types::MirFunction function, mlc::Array<value::VmValue> arguments, bool trace_enabled) noexcept;
outcome::VmRunOutcome vm_failed_outcome(mlc::String message) noexcept;
outcome::VmRunOutcome vm_run_frames_step(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, bool trace_enabled) noexcept;
outcome::VmRunOutcome vm_run_frames(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames_initial, bool trace_enabled) noexcept;
ast::Result<mir_types::MirBlock, mlc::Array<mlc::String>> vm_find_block(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept;
outcome::VmRunOutcome vm_execute_user_call(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::LocalId local_id, mir_types::MirFunction callee, mlc::Array<value::VmValue> argument_values, mlc::String callee_name, bool trace_enabled) noexcept;
outcome::VmRunOutcome vm_execute_statement(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirStmt statement, bool trace_enabled) noexcept;
outcome::VmRunOutcome vm_execute_terminator(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::MirTerminator terminator, bool trace_enabled) noexcept;
mlc::Array<frame::VmFrame> vm_drop_last_frame(mlc::Array<frame::VmFrame> frames) noexcept;
outcome::VmRunOutcome vm_pop_return(mir_types::MirProgram program, mlc::Array<frame::VmFrame> frames, int frame_index, value::VmValue return_value, bool trace_enabled) noexcept;

} // namespace execute

#endif // EXECUTE_HPP
