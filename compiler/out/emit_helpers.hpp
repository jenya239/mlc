#ifndef EMIT_HELPERS_HPP
#define EMIT_HELPERS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_ast.hpp"
namespace emit_helpers {

std::shared_ptr<cpp_ast::CppExpression> make_integer_cpp_expression(int integer_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_string_cpp_expression(mlc::String string_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_boolean_cpp_expression(bool boolean_value) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_identifier_cpp_expression(mlc::String name) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_constexpr_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_fragment_cpp_statement(mlc::String code) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_return_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_expression_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_variable_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppType> type_node, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept;
std::shared_ptr<cpp_ast::CppType> make_size_t_type() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_break_cpp_statement() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_continue_cpp_statement() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_if_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> condition, std::shared_ptr<cpp_ast::CppStatement> then_branch, std::shared_ptr<cpp_ast::CppStatement> else_branch) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_while_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> condition, std::shared_ptr<cpp_ast::CppStatement> body) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_for_cpp_statement(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpression> range_expression, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_structured_binding_cpp_statement(mlc::Array<mlc::String> binding_names, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> make_fragment_cpp_declaration(mlc::String code) noexcept;

} // namespace emit_helpers

#endif // EMIT_HELPERS_HPP
