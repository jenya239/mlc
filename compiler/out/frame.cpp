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

frame::VmFrame vm_frame_new(mir_types::MirFunction function, mir_types::BlockId block_id, mlc::Array<value::VmValue> locals) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_locals_load(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id) noexcept;

mlc::Array<value::VmValue> vm_locals_store(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id, value::VmValue value) noexcept;

frame::VmFrame vm_frame_at(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_update(mlc::Array<frame::VmFrame> frames, int frame_index, frame::VmFrame frame) noexcept;

mlc::Array<frame::VmFrame> vm_frame_advance_statement(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_block(mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::BlockId block_id) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_locals(mlc::Array<frame::VmFrame> frames, int frame_index, mlc::Array<value::VmValue> locals) noexcept;

frame::VmFrame vm_frame_new(mir_types::MirFunction function, mir_types::BlockId block_id, mlc::Array<value::VmValue> locals) noexcept{return frame::VmFrame{function, block_id, 0, locals, -1};}

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_locals_load(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id) noexcept{
int index = mir_ids::mir_local_id_index(local_id);
return index >= 0 && index < locals.size() ? ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Ok<value::VmValue>(locals[index])) : ast::Result<value::VmValue, mlc::Array<mlc::String>>(ast::Err<mlc::Array<mlc::String>>(mlc::Array<mlc::String>{mlc::String("vm: unbound local ") + mlc::to_string(index)}));
}

mlc::Array<value::VmValue> vm_locals_store(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id, value::VmValue value) noexcept{
int index = mir_ids::mir_local_id_index(local_id);
mlc::Array<value::VmValue> slots = locals;
while (slots.size() <= index){
{
slots.push_back(value::vm_value_unit());
}
}
mlc::Array<value::VmValue> rebuilt = {};
int position = 0;
while (position < slots.size()){
{
if (position == index){
{
rebuilt.push_back(value);
}
} else {
{
rebuilt.push_back(slots[position]);
}
}
position = position + 1;
}
}
return rebuilt;
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

} // namespace frame
