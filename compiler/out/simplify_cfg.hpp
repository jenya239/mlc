#ifndef SIMPLIFY_CFG_HPP
#define SIMPLIFY_CFG_HPP

#include "mlc.hpp"
#include <variant>
#include "mir_types.hpp"
#include "mir_ids.hpp"
namespace simplify_cfg {

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

} // namespace simplify_cfg

#endif // SIMPLIFY_CFG_HPP
