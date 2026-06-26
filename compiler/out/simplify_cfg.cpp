#include "simplify_cfg.hpp"

#include "mir_types.hpp"
#include "mir_ids.hpp"

namespace simplify_cfg {

using namespace mir_types;
using namespace mir_ids;

bool mir_block_ids_contains(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept;

mlc::Array<mir_types::BlockId> mir_append_block_id(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept;

mlc::Array<mir_types::BlockId> mir_terminator_target_block_ids(mir_types::MirTerminator terminator) noexcept;

mir_types::MirBlock mir_block_by_id(mlc::Array<mir_types::MirBlock> blocks, mir_types::BlockId block_id) noexcept;

mlc::Array<mir_types::BlockId> mir_collect_reachable_block_ids(mlc::Array<mir_types::MirBlock> blocks) noexcept;

mir_types::MirTerminator mir_resolve_cond_jump_operand(mir_types::MirOperand operand, mir_types::BlockId then_block, mir_types::BlockId else_block) noexcept;

mir_types::MirTerminator mir_resolve_conditional_terminator(mir_types::MirTerminator terminator) noexcept;

mir_types::MirBlock mir_simplify_cfg_block(mir_types::MirBlock block) noexcept;

mlc::Array<mir_types::MirBlock> mir_simplify_cfg_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept;

mlc::Array<mir_types::MirBlock> mir_filter_reachable_blocks(mlc::Array<mir_types::MirBlock> blocks, mlc::Array<mir_types::BlockId> reachable_ids) noexcept;

mir_types::MirFunction simplify_cfg_mir_function(mir_types::MirFunction function) noexcept;

bool mir_block_ids_contains(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept{
int index = 0;
while (index < block_ids.size()){
{
if (mir_ids::mir_block_id_equals(block_ids[index], block_id)){
{
return true;
}
}
index = index + 1;
}
}
return false;
}

mlc::Array<mir_types::BlockId> mir_append_block_id(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept{
if (mir_block_ids_contains(block_ids, block_id)){
{
return block_ids;
}
}
mlc::Array<mir_types::BlockId> next_ids = block_ids;
next_ids.push_back(block_id);
return next_ids;
}

mlc::Array<mir_types::BlockId> mir_terminator_target_block_ids(mir_types::MirTerminator terminator) noexcept{return std::visit(overloaded{
  [&](const MirJump& mirjump) -> mlc::Array<mir_types::BlockId> { auto [target] = mirjump; return mlc::Array<mir_types::BlockId>{target}; },
  [&](const MirCondJump& mircondjump) -> mlc::Array<mir_types::BlockId> { auto [_w0, then_block, else_block] = mircondjump; return mlc::Array<mir_types::BlockId>{then_block, else_block}; },
  [&](const MirReturn& mirreturn) -> mlc::Array<mir_types::BlockId> { auto [_w0] = mirreturn; return {}; },
  [&](const MirUnreachable& mirunreachable) -> mlc::Array<mir_types::BlockId> { return {}; }
}, terminator);}

mir_types::MirBlock mir_block_by_id(mlc::Array<mir_types::MirBlock> blocks, mir_types::BlockId block_id) noexcept{
int index = 0;
while (index < blocks.size()){
{
if (mir_ids::mir_block_id_equals(blocks[index].id, block_id)){
{
return blocks[index];
}
}
index = index + 1;
}
}
return blocks[0];
}

mlc::Array<mir_types::BlockId> mir_collect_reachable_block_ids(mlc::Array<mir_types::MirBlock> blocks) noexcept{
if (blocks.size() == 0){
{
return {};
}
}
mlc::Array<mir_types::BlockId> reachable = mlc::Array<mir_types::BlockId>{blocks[0].id};
int queue_index = 0;
while (queue_index < reachable.size()){
{
mir_types::MirBlock current_block = mir_block_by_id(blocks, reachable[queue_index]);
mlc::Array<mir_types::BlockId> targets = mir_terminator_target_block_ids(current_block.term);
int target_index = 0;
while (target_index < targets.size()){
{
reachable = mir_append_block_id(reachable, targets[target_index]);
target_index = target_index + 1;
}
}
queue_index = queue_index + 1;
}
}
return reachable;
}

mir_types::MirTerminator mir_resolve_cond_jump_operand(mir_types::MirOperand operand, mir_types::BlockId then_block, mir_types::BlockId else_block) noexcept{return [&]() -> mir_types::MirTerminator { if (std::holds_alternative<mir_types::MirOperandConstBool>(operand)) { auto _v_miroperandconstbool = std::get<mir_types::MirOperandConstBool>(operand); auto [condition_value] = _v_miroperandconstbool; return condition_value ? mir_types::MirTerminator(mir_types::MirJump(then_block)) : mir_types::MirTerminator(mir_types::MirJump(else_block)); } return mir_types::MirCondJump(operand, then_block, else_block); }();}

mir_types::MirTerminator mir_resolve_conditional_terminator(mir_types::MirTerminator terminator) noexcept{return [&]() -> mir_types::MirTerminator { if (std::holds_alternative<mir_types::MirCondJump>(terminator)) { auto _v_mircondjump = std::get<mir_types::MirCondJump>(terminator); auto [condition_operand, then_block, else_block] = _v_mircondjump; return mir_resolve_cond_jump_operand(condition_operand, then_block, else_block); } return terminator; }();}

mir_types::MirBlock mir_simplify_cfg_block(mir_types::MirBlock block) noexcept{return mir_types::MirBlock{block.id, block.label, block.stmts, mir_resolve_conditional_terminator(block.term)};}

mlc::Array<mir_types::MirBlock> mir_simplify_cfg_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept{
mlc::Array<mir_types::MirBlock> simplified = {};
int index = 0;
while (index < blocks.size()){
{
simplified.push_back(mir_simplify_cfg_block(blocks[index]));
index = index + 1;
}
}
return simplified;
}

mlc::Array<mir_types::MirBlock> mir_filter_reachable_blocks(mlc::Array<mir_types::MirBlock> blocks, mlc::Array<mir_types::BlockId> reachable_ids) noexcept{
mlc::Array<mir_types::MirBlock> kept = {};
int index = 0;
while (index < blocks.size()){
{
if (mir_block_ids_contains(reachable_ids, blocks[index].id)){
{
kept.push_back(blocks[index]);
}
}
index = index + 1;
}
}
return kept;
}

mir_types::MirFunction simplify_cfg_mir_function(mir_types::MirFunction function) noexcept{
mlc::Array<mir_types::MirBlock> simplified_blocks = mir_simplify_cfg_blocks(function.blocks);
mlc::Array<mir_types::BlockId> reachable_ids = mir_collect_reachable_block_ids(simplified_blocks);
return mir_types::MirFunction{function.name, function.params, function.locals, mir_filter_reachable_blocks(simplified_blocks, reachable_ids), function.return_type};
}

} // namespace simplify_cfg
