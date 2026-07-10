#define main mlc_user_main
#include "const_fold.hpp"

#include "mir_types.hpp"

namespace const_fold {

using namespace mir_types;

mir_types::MirOperand const_fold_operand(mir_types::MirOperand operand) noexcept{
  return operand;
}
mir_types::MirOperand const_fold_binary_int(mlc::String operation, int left, int right) noexcept{
  if ((operation == mlc::String("+", 1)))   {
    return mir_types::MirOperandConstInt{mlc::arith::checked_add(left, right)};
  }
  if ((operation == mlc::String("-", 1)))   {
    return mir_types::MirOperandConstInt{mlc::arith::checked_sub(left, right)};
  }
  if ((operation == mlc::String("*", 1)))   {
    return mir_types::MirOperandConstInt{mlc::arith::checked_mul(left, right)};
  }
  if (((operation == mlc::String("/", 1)) && (right != 0)))   {
    return mir_types::MirOperandConstInt{mlc::arith::checked_div(left, right)};
  }
  if (((operation == mlc::String("%", 1)) && (right != 0)))   {
    return mir_types::MirOperandConstInt{mlc::arith::checked_mod(left, right)};
  }
  if ((operation == mlc::String(">", 1)))   {
    return mir_types::MirOperandConstBool{(left > right)};
  }
  if ((operation == mlc::String("<", 1)))   {
    return mir_types::MirOperandConstBool{(left < right)};
  }
  if ((operation == mlc::String(">=", 2)))   {
    return mir_types::MirOperandConstBool{(left >= right)};
  }
  if ((operation == mlc::String("<=", 2)))   {
    return mir_types::MirOperandConstBool{(left <= right)};
  }
  if ((operation == mlc::String("==", 2)))   {
    return mir_types::MirOperandConstBool{(left == right)};
  }
  if ((operation == mlc::String("!=", 2)))   {
    return mir_types::MirOperandConstBool{(left != right)};
  }
  return mir_types::MirOperandConstInt{0};
}
bool mir_operand_is_const_int(mir_types::MirOperand operand) noexcept{
  return [&]() -> bool {
auto __match_subject = operand;
if (std::holds_alternative<mir_types::MirOperandConstInt>(__match_subject)) {
const mir_types::MirOperandConstInt& mirOperandConstInt = std::get<mir_types::MirOperandConstInt>(__match_subject);
auto [__0] = mirOperandConstInt; return true;
}
return false;
std::abort();
}();
}
int mir_operand_const_int_value(mir_types::MirOperand operand) noexcept{
  return [&]() -> int {
auto __match_subject = operand;
if (std::holds_alternative<mir_types::MirOperandConstInt>(__match_subject)) {
const mir_types::MirOperandConstInt& mirOperandConstInt = std::get<mir_types::MirOperandConstInt>(__match_subject);
auto [value] = mirOperandConstInt; return value;
}
return 0;
std::abort();
}();
}
mir_types::MirRvalue const_fold_int_pair(mlc::String operation, int left_value, int right_value) noexcept{
  return mir_types::MirRvalueUse{const_fold_binary_int(operation, left_value, right_value)};
}
mir_types::MirRvalue const_fold_rvalue_binary(mlc::String operation, mir_types::MirOperand left, mir_types::MirOperand right) noexcept{
  auto folded_left = const_fold_operand(left);
  auto folded_right = const_fold_operand(right);
  if ((mir_operand_is_const_int(folded_left) && mir_operand_is_const_int(folded_right)))   {
    return const_fold_int_pair(operation, mir_operand_const_int_value(folded_left), mir_operand_const_int_value(folded_right));
  }
  return mir_types::MirRvalueBinary{operation, folded_left, folded_right};
}
mir_types::MirRvalue const_fold_rvalue(mir_types::MirRvalue rvalue) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirRvalueUse& mirRvalueUse) -> mir_types::MirRvalue { auto [operand] = mirRvalueUse; return mir_types::MirRvalueUse{const_fold_operand(operand)}; },
[&](const mir_types::MirRvalueBinary& mirRvalueBinary) -> mir_types::MirRvalue { auto [operation, left, right] = mirRvalueBinary; return const_fold_rvalue_binary(operation, left, right); },
[&](const mir_types::MirRvalueUnary& mirRvalueUnary) -> mir_types::MirRvalue { auto [operation, operand] = mirRvalueUnary; return mir_types::MirRvalueUnary{operation, const_fold_operand(operand)}; }
}, rvalue);
}
mlc::Array<mir_types::MirOperand> const_fold_call_operands(mlc::Array<mir_types::MirOperand> operands) noexcept{
  auto folded = mlc::Array<mir_types::MirOperand>{};
  auto index = 0;
  while ((index < operands.length()))   {
    folded.push_back(const_fold_operand(operands[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return folded;
}
mir_types::MirStmt const_fold_statement(mir_types::MirStmt statement) noexcept{
  return std::visit(overloaded{[&](const mir_types::MirAssign& mirAssign) -> mir_types::MirStmt { auto [local_id, rvalue] = mirAssign; return mir_types::MirAssign{local_id, const_fold_rvalue(rvalue)}; },
[&](const mir_types::MirCallAssign& mirCallAssign) -> mir_types::MirStmt { auto [local_id, callee_name, arguments] = mirCallAssign; return mir_types::MirCallAssign{local_id, callee_name, const_fold_call_operands(arguments)}; }
}, statement);
}
mlc::Array<mir_types::MirStmt> const_fold_block_statements(mlc::Array<mir_types::MirStmt> statements) noexcept{
  auto folded = mlc::Array<mir_types::MirStmt>{};
  auto index = 0;
  while ((index < statements.length()))   {
    folded.push_back(const_fold_statement(statements[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return folded;
}
mir_types::MirTerminator const_fold_terminator(mir_types::MirTerminator terminator) noexcept{
  return [&]() -> mir_types::MirTerminator {
auto __match_subject = terminator;
if (std::holds_alternative<mir_types::MirReturn>(__match_subject)) {
const mir_types::MirReturn& mirReturn = std::get<mir_types::MirReturn>(__match_subject);
auto [operand] = mirReturn; return mir_types::MirReturn{const_fold_operand(operand)};
}
if (std::holds_alternative<mir_types::MirCondJump>(__match_subject)) {
const mir_types::MirCondJump& mirCondJump = std::get<mir_types::MirCondJump>(__match_subject);
auto [operand, then_block, else_block] = mirCondJump; return mir_types::MirCondJump{const_fold_operand(operand), then_block, else_block};
}
return terminator;
std::abort();
}();
}
mir_types::MirBlock const_fold_block(mir_types::MirBlock block) noexcept{
  return mir_types::MirBlock{block.id, block.label, const_fold_block_statements(block.stmts), const_fold_terminator(block.term)};
}
mlc::Array<mir_types::MirBlock> const_fold_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept{
  auto folded = mlc::Array<mir_types::MirBlock>{};
  auto index = 0;
  while ((index < blocks.length()))   {
    folded.push_back(const_fold_block(blocks[index]));
    (index = mlc::arith::checked_add(index, 1));
  }
  return folded;
}
mir_types::MirFunction const_fold_mir_function(mir_types::MirFunction function) noexcept{
  return mir_types::MirFunction{function.name, function.params, function.locals, const_fold_blocks(function.blocks), function.return_type};
}

} // namespace const_fold
