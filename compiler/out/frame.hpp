#ifndef FRAME_HPP
#define FRAME_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"
#include "mir_ids.hpp"
#include "value.hpp"
#include "ast.hpp"

namespace frame {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct VmFrame {mir_types::MirFunction function;mir_types::BlockId block_id;int statement_index;mlc::Array<value::VmValue> locals;int pending_call_local;};

frame::VmFrame vm_frame_new(mir_types::MirFunction function, mir_types::BlockId block_id, mlc::Array<value::VmValue> locals) noexcept;

ast::Result<value::VmValue, mlc::Array<mlc::String>> vm_locals_load(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id) noexcept;

mlc::Array<value::VmValue> vm_locals_store(mlc::Array<value::VmValue> locals, mir_types::LocalId local_id, value::VmValue value) noexcept;

frame::VmFrame vm_frame_at(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_update(mlc::Array<frame::VmFrame> frames, int frame_index, frame::VmFrame frame) noexcept;

mlc::Array<frame::VmFrame> vm_frame_advance_statement(mlc::Array<frame::VmFrame> frames, int frame_index) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_block(mlc::Array<frame::VmFrame> frames, int frame_index, mir_types::BlockId block_id) noexcept;

mlc::Array<frame::VmFrame> vm_frame_set_locals(mlc::Array<frame::VmFrame> frames, int frame_index, mlc::Array<value::VmValue> locals) noexcept;

} // namespace frame

#endif // FRAME_HPP
