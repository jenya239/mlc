#include "const_fold.hpp"

#include "mir_types.hpp"

namespace const_fold {

using namespace mir_types;

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

mir_types::MirOperand const_fold_operand(mir_types::MirOperand operand) noexcept{return operand;}

mir_types::MirOperand const_fold_binary_int(mlc::String operation, int left, int right) noexcept{
if (operation == mlc::String("+")){
{
return mir_types::MirOperandConstInt(left + right);
}
}
if (operation == mlc::String("-")){
{
return mir_types::MirOperandConstInt(left - right);
}
}
if (operation == mlc::String("*")){
{
return mir_types::MirOperandConstInt(left * right);
}
}
if (operation == mlc::String("/") && right != 0){
{
return mir_types::MirOperandConstInt(left / right);
}
}
if (operation == mlc::String("%") && right != 0){
{
return mir_types::MirOperandConstInt(left % right);
}
}
if (operation == mlc::String(">")){
{
return mir_types::MirOperandConstBool(left > right);
}
}
if (operation == mlc::String("<")){
{
return mir_types::MirOperandConstBool(left < right);
}
}
if (operation == mlc::String(">=")){
{
return mir_types::MirOperandConstBool(left >= right);
}
}
if (operation == mlc::String("<=")){
{
return mir_types::MirOperandConstBool(left <= right);
}
}
if (operation == mlc::String("==")){
{
return mir_types::MirOperandConstBool(left == right);
}
}
if (operation == mlc::String("!=")){
{
return mir_types::MirOperandConstBool(left != right);
}
}
return mir_types::MirOperandConstInt(0);
}

bool mir_operand_is_const_int(mir_types::MirOperand operand) noexcept{return [&]() { if (std::holds_alternative<mir_types::MirOperandConstInt>(operand)) { auto _v_miroperandconstint = std::get<mir_types::MirOperandConstInt>(operand); auto [_w0] = _v_miroperandconstint; return true; } return false; }();}

int mir_operand_const_int_value(mir_types::MirOperand operand) noexcept{return [&]() { if (std::holds_alternative<mir_types::MirOperandConstInt>(operand)) { auto _v_miroperandconstint = std::get<mir_types::MirOperandConstInt>(operand); auto [value] = _v_miroperandconstint; return value; } return 0; }();}

mir_types::MirRvalue const_fold_int_pair(mlc::String operation, int left_value, int right_value) noexcept{return mir_types::MirRvalueUse(const_fold_binary_int(operation, left_value, right_value));}

mir_types::MirRvalue const_fold_rvalue_binary(mlc::String operation, mir_types::MirOperand left, mir_types::MirOperand right) noexcept{
mir_types::MirOperand folded_left = const_fold_operand(left);
mir_types::MirOperand folded_right = const_fold_operand(right);
if (mir_operand_is_const_int(folded_left) && mir_operand_is_const_int(folded_right)){
{
return const_fold_int_pair(operation, mir_operand_const_int_value(folded_left), mir_operand_const_int_value(folded_right));
}
}
return mir_types::MirRvalueBinary(operation, folded_left, folded_right);
}

mir_types::MirRvalue const_fold_rvalue(mir_types::MirRvalue rvalue) noexcept{return std::visit(overloaded{
  [&](const MirRvalueUse& mirrvalueuse) -> mir_types::MirRvalue { auto [operand] = mirrvalueuse; return mir_types::MirRvalueUse(const_fold_operand(operand)); },
  [&](const MirRvalueBinary& mirrvaluebinary) -> mir_types::MirRvalue { auto [operation, left, right] = mirrvaluebinary; return const_fold_rvalue_binary(operation, left, right); }
}, rvalue);}

mlc::Array<mir_types::MirOperand> const_fold_call_operands(mlc::Array<mir_types::MirOperand> operands) noexcept{
mlc::Array<mir_types::MirOperand> folded = {};
int index = 0;
while (index < operands.size()){
{
folded.push_back(const_fold_operand(operands[index]));
index = index + 1;
}
}
return folded;
}

mir_types::MirStmt const_fold_statement(mir_types::MirStmt statement) noexcept{return std::visit(overloaded{
  [&](const MirAssign& mirassign) -> mir_types::MirStmt { auto [local_id, rvalue] = mirassign; return mir_types::MirAssign(local_id, const_fold_rvalue(rvalue)); },
  [&](const MirCallAssign& mircallassign) -> mir_types::MirStmt { auto [local_id, callee_name, arguments] = mircallassign; return mir_types::MirCallAssign(local_id, callee_name, const_fold_call_operands(arguments)); }
}, statement);}

mlc::Array<mir_types::MirStmt> const_fold_block_statements(mlc::Array<mir_types::MirStmt> statements) noexcept{
mlc::Array<mir_types::MirStmt> folded = {};
int index = 0;
while (index < statements.size()){
{
folded.push_back(const_fold_statement(statements[index]));
index = index + 1;
}
}
return folded;
}

mir_types::MirTerminator const_fold_terminator(mir_types::MirTerminator terminator) noexcept{return [&]() -> mir_types::MirTerminator { if (std::holds_alternative<mir_types::MirReturn>(terminator)) { auto _v_mirreturn = std::get<mir_types::MirReturn>(terminator); auto [operand] = _v_mirreturn; return mir_types::MirReturn(const_fold_operand(operand)); } if (std::holds_alternative<mir_types::MirCondJump>(terminator)) { auto _v_mircondjump = std::get<mir_types::MirCondJump>(terminator); auto [operand, then_block, else_block] = _v_mircondjump; return mir_types::MirCondJump(const_fold_operand(operand), then_block, else_block); } return terminator; }();}

mir_types::MirBlock const_fold_block(mir_types::MirBlock block) noexcept{return mir_types::MirBlock{block.id, block.label, const_fold_block_statements(block.stmts), const_fold_terminator(block.term)};}

mlc::Array<mir_types::MirBlock> const_fold_blocks(mlc::Array<mir_types::MirBlock> blocks) noexcept{
mlc::Array<mir_types::MirBlock> folded = {};
int index = 0;
while (index < blocks.size()){
{
folded.push_back(const_fold_block(blocks[index]));
index = index + 1;
}
}
return folded;
}

mir_types::MirFunction const_fold_mir_function(mir_types::MirFunction function) noexcept{return mir_types::MirFunction{function.name, function.params, function.locals, const_fold_blocks(function.blocks), function.return_type};}

} // namespace const_fold
