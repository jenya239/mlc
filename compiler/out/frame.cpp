#define main mlc_user_main
#include "frame.hpp"

#include "mir_types.hpp"
#include "mir_ids.hpp"
#include "value.hpp"
#include "ast.hpp"

namespace frame {

using namespace mir_types;
using namespace mir_ids;
using namespace value;
using namespace ast;
using namespace ast_tokens;

VmFrame vm_frame_new(mir_types::MirFunction function, mir_types::BlockId block_id, mlc::Array<value::VmValue> locals) noexcept{
  return VmFrame{function, block_id, 0, locals, (-1)};
}
ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_locals_load(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id) noexcept{
  auto index = mir_ids::mir_local_id_index(local_id);
  if (((index >= 0) && (index < locals.length())))   {
    return ast::Ok<value::VmValue>{locals[index]};
  } else   {
    return ast::Err<mlc::Array<mlc::String>>{mlc::Array<mlc::String>{((mlc::String("vm: unbound local ", 18) + mlc::to_string(index)) + mlc::String("", 0))}};
  }
}
mlc::Array<value::VmValue> vm_locals_store(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id, value::VmValue value) noexcept{
  auto index = mir_ids::mir_local_id_index(local_id);
  auto slots = locals;
  while ((slots.length() <= index))   {
    slots.push_back(value::vm_value_unit());
  }
  auto rebuilt = mlc::Array<value::VmValue>{};
  auto position = 0;
  while ((position < slots.length()))   {
    if ((position == index))     {
      rebuilt.push_back(value);
    } else     {
      rebuilt.push_back(slots[position]);
    }
    (position = (position + 1));
  }
  return rebuilt;
}
VmFrame vm_frame_at(mlc::Array<VmFrame> frames, int frame_index) noexcept{
  return frames[frame_index];
}
mlc::Array<VmFrame> vm_frame_update(mlc::Array<VmFrame> frames, int frame_index, VmFrame frame) noexcept{
  auto rebuilt = mlc::Array<VmFrame>{};
  auto index = 0;
  while ((index < frames.length()))   {
    if ((index == frame_index))     {
      rebuilt.push_back(frame);
    } else     {
      rebuilt.push_back(frames[index]);
    }
    (index = (index + 1));
  }
  return rebuilt;
}
mlc::Array<VmFrame> vm_frame_advance_statement(mlc::Array<VmFrame> frames, int frame_index) noexcept{
  auto frame = vm_frame_at(frames, frame_index);
  return vm_frame_update(frames, frame_index, VmFrame{frame.function, frame.block_id, (frame.statement_index + 1), frame.locals, frame.pending_call_local});
}
mlc::Array<VmFrame> vm_frame_set_block(mlc::Array<VmFrame> frames, int frame_index, mir_types::BlockId block_id) noexcept{
  auto frame = vm_frame_at(frames, frame_index);
  return vm_frame_update(frames, frame_index, VmFrame{frame.function, block_id, 0, frame.locals, frame.pending_call_local});
}
mlc::Array<VmFrame> vm_frame_set_locals(mlc::Array<VmFrame> frames, int frame_index, mlc::Array<value::VmValue> locals) noexcept{
  auto frame = vm_frame_at(frames, frame_index);
  return vm_frame_update(frames, frame_index, VmFrame{frame.function, frame.block_id, frame.statement_index, locals, frame.pending_call_local});
}

} // namespace frame
