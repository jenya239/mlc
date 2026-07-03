#define main mlc_user_main
#include "simplify_cfg.hpp"

#include "mir_types.hpp"
#include "mir_ids.hpp"

namespace simplify_cfg {

using namespace mir_types;
using namespace mir_ids;

bool mir_block_ids_contains(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept{
  auto index = 0;
  while ((index < block_ids.length()))   {
    if (mir_ids::mir_block_id_equals(block_ids[index], block_id))     {
      return true;
    }
    (index = (index + 1));
  }
  return false;
}
mlc::Array<mir_types::BlockId> mir_append_block_id(mlc::Array<mir_types::BlockId> block_ids, mir_types::BlockId block_id) noexcept{
  if (mir_block_ids_contains(block_ids, block_id))   {
    return block_ids;
  }
  auto next_ids = block_ids;
  next_ids.push_back(block_id);
  return next_ids;
}
mlc::Array<mir_types::BlockId> mir_terminator_target_block_ids(mir_types::MirTerminator terminator) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirJump& mirJump) { auto [target] = mirJump; return mlc::Array<mir_types::BlockId>{target}; },
[&](const mir_types::MirCondJump& mirCondJump) { auto [__0, then_block, else_block] = mirCondJump; return mlc::Array<mir_types::BlockId>{then_block, else_block}; },
[&](const mir_types::MirReturn& mirReturn) { auto [__0] = mirReturn; return mlc::Array<mir_types::BlockId>{}; },
[&](const mir_types::MirUnreachable& mirUnreachable) { return mlc::Array<mir_types::BlockId>{}; }
}, terminator);
}
mir_types::MirBlock mir_block_by_id(mlc::Array<mir_types::MirBlock> blocks, mir_types::BlockId block_id) noexcept{
  auto index = 0;
  while ((index < blocks.length()))   {
    if (mir_ids::mir_block_id_equals(blocks[index].id, block_id))     {
      return blocks[index];
    }
    (index = (index + 1));
  }
  return blocks[0];
}
mlc::Array<mir_types::BlockId> mir_collect_reachable_block_ids(mlc::Array<mir_types::MirBlock> blocks) noexcept{
  if ((blocks.length() == 0))   {
    return {};
  }
  auto reachable = mlc::Array<mir_types::BlockId>{blocks[0].id};
  auto queue_index = 0;
  while ((queue_index < reachable.length()))   {
    auto current_block = mir_block_by_id(blocks, reachable[queue_index]);
    auto targets = mir_terminator_target_block_ids(current_block.term);
    auto target_index = 0;
    while ((target_index < targets.length()))     {
      (reachable = mir_append_block_id(reachable, targets[target_index]));
      (target_index = (target_index + 1));
    }
    (queue_index = (queue_index + 1));
  }
  return reachable;
}
mir_types::MirTerminator mir_resolve_cond_jump_operand(mir_types::MirOperand operand, mir_types::BlockId then_block, mir_types::BlockId else_block) noexcept{
  return [&]() -> mir_types::MirTerminator {
auto __match_subject = operand;
if (std::holds_alternative<mir_types::MirOperandConstBool>(__match_subject)) {
const mir_types::MirOperandConstBool& mirOperandConstBool = std::get<mir_types::MirOperandConstBool>(__match_subject);
auto [condition_value] = mirOperandConstBool; return [&]() -> mir_types::MirTerminator {
  if (condition_value)   {
    return mir_types::MirJump{then_block};
  } else   {
    return mir_types::MirJump{else_block};
  }
}();
}
return mir_types::MirCondJump{operand, then_block, else_block};
std::abort();
}();
}
mir_types::MirTerminator mir_resolve_conditional_terminator(mir_types::MirTerminator terminator) noexcept{
  return [&]() -> mir_types::MirTerminator {
auto __match_subject = terminator;
if (std::holds_alternative<mir_types::MirCondJump>(__match_subject)) {
const mir_types::MirCondJump& mirCondJump = std::get<mir_types::MirCondJump>(__match_subject);
auto [condition_operand, then_block, else_block] = mirCondJump; return mir_resolve_cond_jump_operand(condition_operand, then_block, else_block);
}
return terminator;
std::abort();
}();
}
mir_types::MirBlock mir_simplify_cfg_block(mir_types::MirBlock block) noexcept{
  return mir_types::MirBlock{block.id, block.label, block.stmts, mir_resolve_conditional_terminator(block.term)};
}
mlc::Array<mir_types::MirBlock> mir_simplify_cfg_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept{
  auto simplified = mlc::Array<mir_types::MirBlock>{};
  auto index = 0;
  while ((index < blocks.length()))   {
    simplified.push_back(mir_simplify_cfg_block(blocks[index]));
    (index = (index + 1));
  }
  return simplified;
}
mlc::Array<mir_types::MirBlock> mir_filter_reachable_blocks(mlc::Array<mir_types::MirBlock> blocks, mlc::Array<mir_types::BlockId> reachable_ids) noexcept{
  auto kept = mlc::Array<mir_types::MirBlock>{};
  auto index = 0;
  while ((index < blocks.length()))   {
    if (mir_block_ids_contains(reachable_ids, blocks[index].id))     {
      kept.push_back(blocks[index]);
    }
    (index = (index + 1));
  }
  return kept;
}
mir_types::MirFunction simplify_cfg_mir_function(mir_types::MirFunction function) noexcept{
  auto simplified_blocks = mir_simplify_cfg_blocks(function.blocks);
  auto reachable_ids = mir_collect_reachable_block_ids(simplified_blocks);
  return mir_types::MirFunction{function.name, function.params, function.locals, mir_filter_reachable_blocks(simplified_blocks, reachable_ids), function.return_type};
}

} // namespace simplify_cfg
