#ifndef EMIT_HELPERS_HPP
#define EMIT_HELPERS_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"

namespace emit_helpers {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

std::shared_ptr<cpp_ast::CppExpr> make_integer_cpp_expression(int integer_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_string_cpp_expression(mlc::String string_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_boolean_cpp_expression(bool boolean_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_identifier_cpp_expression(mlc::String name) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_constexpr_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpr> initializer) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_fragment_cpp_statement(mlc::String code) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpr> initializer) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_return_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_expression_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_break_cpp_statement() noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_continue_cpp_statement() noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_if_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> condition, std::shared_ptr<cpp_ast::CppStmt> then_branch, std::shared_ptr<cpp_ast::CppStmt> else_branch) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_while_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> condition, std::shared_ptr<cpp_ast::CppStmt> body) noexcept;

std::shared_ptr<cpp_ast::CppStmt> make_for_cpp_statement(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpr> range_expression, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> body) noexcept;

std::shared_ptr<cpp_ast::CppDecl> make_fragment_cpp_declaration(mlc::String code) noexcept;

} // namespace emit_helpers

#endif // EMIT_HELPERS_HPP
