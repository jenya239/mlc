#ifndef CORE_HPP
#define CORE_HPP

#include "mlc.hpp"
#include <variant>

#include "registry.hpp"

namespace core {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CoreNodeId {int value;};

struct CoreTerminatorReturn {core::CoreNodeId field0;};struct CoreTerminatorUnreachable {};using CoreTerminator = std::variant<CoreTerminatorReturn, CoreTerminatorUnreachable>;

struct CoreExpressionLiteralInt {int field0;std::shared_ptr<registry::Type> field1;};struct CoreExpressionLocal {mlc::String field0;std::shared_ptr<registry::Type> field1;};struct CoreExpressionCall {mlc::String field0;mlc::Array<core::CoreNodeId> field1;std::shared_ptr<registry::Type> field2;};using CoreExpression = std::variant<CoreExpressionLiteralInt, CoreExpressionLocal, CoreExpressionCall>;

struct CoreStatementLocalBind {mlc::String field0;core::CoreExpression field1;};struct CoreStatementExpression {core::CoreExpression field0;};using CoreStatement = std::variant<CoreStatementLocalBind, CoreStatementExpression>;

struct CoreBasicBlock {core::CoreNodeId identifier;mlc::String label;mlc::Array<core::CoreStatement> statements;core::CoreTerminator terminator;};

struct CoreFunction {mlc::String name;mlc::Array<mlc::String> parameters;mlc::Array<core::CoreBasicBlock> blocks;core::CoreNodeId entry_block;};

struct CoreModule {mlc::Array<core::CoreFunction> functions;};

core::CoreNodeId core_node_id_new(int value) noexcept;

bool core_node_id_is_valid(core::CoreNodeId identifier) noexcept;

core::CoreModule core_module_empty() noexcept;

bool core_function_name_is_safe(mlc::String name) noexcept;

bool core_block_label_is_safe(mlc::String label) noexcept;

} // namespace core

#endif // CORE_HPP
