#ifndef MIR_TO_CPP_HPP
#define MIR_TO_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "semantic_type_structure.hpp"
#include "mir_types.hpp"
#include "mir_ids.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
namespace mir_to_cpp {

std::shared_ptr<registry::Type> mir_to_cpp_unknown_type() noexcept;
mlc::String mir_cpp_safe_name(mlc::String name) noexcept;
mlc::String mir_temp_local_name(mir_types::LocalId local_id) noexcept;
mlc::String mir_type_to_cpp_string(std::shared_ptr<registry::Type> type_value) noexcept;
mir_types::MirLocal mir_find_local(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept;
mir_types::MirBlock mir_block_by_id(mir_types::MirFunction function, mir_types::BlockId block_id) noexcept;
mlc::String mir_local_cpp_name(mir_types::MirFunction function, mir_types::LocalId local_id) noexcept;
std::shared_ptr<cpp_ast::CppExpression> mir_operand_to_cpp_expression(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept;
std::shared_ptr<registry::Type> mir_operand_type(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept;
std::shared_ptr<cpp_ast::CppExpression> mir_rvalue_to_cpp_expression(mir_types::MirFunction function, mir_types::MirRvalue rvalue) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> mir_call_operands_to_cpp_expressions(mir_types::MirFunction function, mlc::Array<mir_types::MirOperand> operands) noexcept;
std::shared_ptr<cpp_ast::CppStatement> mir_stmt_to_cpp_statement(mir_types::MirFunction function, mir_types::MirStmt statement) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_stmts_to_cpp_statements(mir_types::MirFunction function, mlc::Array<mir_types::MirStmt> statements) noexcept;
std::shared_ptr<cpp_ast::CppStatement> mir_return_to_cpp_statement(mir_types::MirFunction function, mir_types::MirOperand operand) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_block_to_cpp_statements(mir_types::MirFunction function, mir_types::MirBlock block) noexcept;
bool mir_function_is_single_block_simple(mir_types::MirFunction function) noexcept;
bool mir_function_is_conditional_simple(mir_types::MirFunction function) noexcept;
bool mir_function_is_simple(mir_types::MirFunction function) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_single_block_to_cpp_statements(mir_types::MirFunction function) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_conditional_to_cpp_statements(mir_types::MirFunction function) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> mir_function_to_cpp_statements(mir_types::MirFunction function) noexcept;
mlc::Array<mlc::String> mir_function_parameter_strings(mir_types::MirFunction function) noexcept;
ast::Result<std::shared_ptr<cpp_ast::CppDeclaration>, mlc::Array<mlc::String>> mir_function_to_cpp_declaration(mir_types::MirFunction function) noexcept;

} // namespace mir_to_cpp

#endif // MIR_TO_CPP_HPP
