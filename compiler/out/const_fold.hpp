#ifndef CONST_FOLD_HPP
#define CONST_FOLD_HPP

#include "mlc.hpp"
#include <variant>
#include "mir_types.hpp"
namespace const_fold {

mir_types::MirOperand const_fold_operand(mir_types::MirOperand operand) noexcept;
mir_types::MirOperand const_fold_binary_int(mlc::String operation, int left, int right) noexcept;
bool mir_operand_is_const_int(mir_types::MirOperand operand) noexcept;
int mir_operand_const_int_value(mir_types::MirOperand operand) noexcept;
mir_types::MirRvalue const_fold_int_pair(mlc::String operation, int left_value, int right_value) noexcept;
mir_types::MirRvalue const_fold_rvalue_binary(mlc::String operation, mir_types::MirOperand left, mir_types::MirOperand right) noexcept;
mir_types::MirRvalue const_fold_rvalue(mir_types::MirRvalue rvalue) noexcept;
mlc::Array<mir_types::MirOperand> const_fold_call_operands(mlc::Array<mir_types::MirOperand> operands) noexcept;
mir_types::MirStmt const_fold_statement(mir_types::MirStmt statement) noexcept;
mlc::Array<mir_types::MirStmt> const_fold_block_statements(mlc::Array<mir_types::MirStmt> statements) noexcept;
mir_types::MirTerminator const_fold_terminator(mir_types::MirTerminator terminator) noexcept;
mir_types::MirBlock const_fold_block(mir_types::MirBlock block) noexcept;
mlc::Array<mir_types::MirBlock> const_fold_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept;
mir_types::MirFunction const_fold_mir_function(mir_types::MirFunction function) noexcept;

} // namespace const_fold

#endif // CONST_FOLD_HPP
