#ifndef MIR_TYPES_HPP
#define MIR_TYPES_HPP

#include "mlc.hpp"
#include <variant>
#include "registry.hpp"
namespace mir_types {

struct MirOperandLocal;
struct MirOperandConstInt;
struct MirOperandConstBool;
struct MirOperandConstStr;
struct MirOperandUnit;
using MirOperand = std::variant<MirOperandLocal, MirOperandConstInt, MirOperandConstBool, MirOperandConstStr, MirOperandUnit>;
struct MirRvalueUse;
struct MirRvalueBinary;
using MirRvalue = std::variant<MirRvalueUse, MirRvalueBinary>;
struct MirAssign;
struct MirCallAssign;
using MirStmt = std::variant<MirAssign, MirCallAssign>;
struct MirReturn;
struct MirJump;
struct MirCondJump;
struct MirUnreachable;
using MirTerminator = std::variant<MirReturn, MirJump, MirCondJump, MirUnreachable>;
struct LocalId {
  int index;
};
struct BlockId {
  int index;
};
struct MirParam {
  mlc::String name;
  std::shared_ptr<registry::Type> type_value;
};
struct MirLocal {
  LocalId id;
  mlc::String name;
  std::shared_ptr<registry::Type> type_value;
};
struct MirOperandLocal {
  LocalId field0;
};
struct MirOperandConstInt {
  int field0;
};
struct MirOperandConstBool {
  bool field0;
};
struct MirOperandConstStr {
  mlc::String field0;
};
struct MirOperandUnit {};
struct MirRvalueUse {
  MirOperand field0;
};
struct MirRvalueBinary {
  mlc::String field0;
  MirOperand field1;
  MirOperand field2;
};
struct MirAssign {
  LocalId field0;
  MirRvalue field1;
};
struct MirCallAssign {
  LocalId field0;
  mlc::String field1;
  mlc::Array<MirOperand> field2;
};
struct MirReturn {
  MirOperand field0;
};
struct MirJump {
  BlockId field0;
};
struct MirCondJump {
  MirOperand field0;
  BlockId field1;
  BlockId field2;
};
struct MirUnreachable {};
struct MirBlock {
  BlockId id;
  mlc::String label;
  mlc::Array<MirStmt> stmts;
  MirTerminator term;
};
struct MirFunction {
  mlc::String name;
  mlc::Array<MirParam> params;
  mlc::Array<MirLocal> locals;
  mlc::Array<MirBlock> blocks;
  std::shared_ptr<registry::Type> return_type;
};
struct MirModule {
  mlc::Array<MirFunction> functions;
};
struct MirProgram {
  mlc::Array<MirModule> modules;
};
LocalId mir_local_id(int index) noexcept;
BlockId mir_block_id(int index) noexcept;
bool mir_id_is_valid(int index) noexcept;
MirProgram mir_program_empty() noexcept;
bool mir_name_character_is_safe(mlc::String character) noexcept;
bool mir_function_name_is_safe(mlc::String name) noexcept;
bool mir_block_label_is_safe(mlc::String label) noexcept;

} // namespace mir_types

#endif // MIR_TYPES_HPP
