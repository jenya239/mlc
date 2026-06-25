#ifndef MIR_TYPES_HPP
#define MIR_TYPES_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace mir_types {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct LocalId {int index;};

struct BlockId {int index;};

struct MirParam {mlc::String name;std::shared_ptr<registry::Type> type_value;};

struct MirLocal {mir_types::LocalId id;mlc::String name;std::shared_ptr<registry::Type> type_value;};

struct MirOperandLocal {mir_types::LocalId field0;};struct MirOperandConstInt {int field0;};struct MirOperandConstBool {bool field0;};struct MirOperandUnit {};using MirOperand = std::variant<MirOperandLocal, MirOperandConstInt, MirOperandConstBool, MirOperandUnit>;

struct MirRvalueUse {mir_types::MirOperand field0;};struct MirRvalueBinary {mlc::String field0;mir_types::MirOperand field1;mir_types::MirOperand field2;};using MirRvalue = std::variant<MirRvalueUse, MirRvalueBinary>;

struct MirAssign {mir_types::LocalId field0;mir_types::MirRvalue field1;};struct MirCallAssign {mir_types::LocalId field0;mlc::String field1;mlc::Array<mir_types::MirOperand> field2;};using MirStmt = std::variant<MirAssign, MirCallAssign>;

struct MirReturn {mir_types::MirOperand field0;};struct MirJump {mir_types::BlockId field0;};struct MirCondJump {mir_types::MirOperand field0;mir_types::BlockId field1;mir_types::BlockId field2;};struct MirUnreachable {};using MirTerminator = std::variant<MirReturn, MirJump, MirCondJump, MirUnreachable>;

struct MirBlock {mir_types::BlockId id;mlc::String label;mlc::Array<mir_types::MirStmt> stmts;mir_types::MirTerminator term;};

struct MirFunction {mlc::String name;mlc::Array<mir_types::MirParam> params;mlc::Array<mir_types::MirLocal> locals;mlc::Array<mir_types::MirBlock> blocks;std::shared_ptr<registry::Type> return_type;};

struct MirModule {mlc::Array<mir_types::MirFunction> functions;};

struct MirProgram {mlc::Array<mir_types::MirModule> modules;};

mir_types::LocalId mir_local_id(int index) noexcept;

mir_types::BlockId mir_block_id(int index) noexcept;

bool mir_id_is_valid(int index) noexcept;

mir_types::MirProgram mir_program_empty() noexcept;

bool mir_function_name_is_safe(mlc::String name) noexcept;

bool mir_block_label_is_safe(mlc::String label) noexcept;

} // namespace mir_types

#endif // MIR_TYPES_HPP
