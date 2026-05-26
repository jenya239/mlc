#include "emit_helpers.hpp"

#include "cpp_ast.hpp"

namespace emit_helpers {

using namespace cpp_ast;

std::shared_ptr<cpp_ast::CppExpr> make_integer_cpp_expression(int integer_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_string_cpp_expression(mlc::String string_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_boolean_cpp_expression(bool boolean_value) noexcept;

std::shared_ptr<cpp_ast::CppExpr> make_identifier_cpp_expression(mlc::String name) noexcept;

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

std::shared_ptr<cpp_ast::CppExpr> make_integer_cpp_expression(int integer_value) noexcept{return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppInt(integer_value));}

std::shared_ptr<cpp_ast::CppExpr> make_string_cpp_expression(mlc::String string_value) noexcept{return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppStr(string_value));}

std::shared_ptr<cpp_ast::CppExpr> make_boolean_cpp_expression(bool boolean_value) noexcept{return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppBool(boolean_value));}

std::shared_ptr<cpp_ast::CppExpr> make_identifier_cpp_expression(mlc::String name) noexcept{return std::make_shared<cpp_ast::CppExpr>(cpp_ast::CppIdent(name));}

std::shared_ptr<cpp_ast::CppStmt> make_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpr> initializer) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppAutoDecl(name, initializer));}

std::shared_ptr<cpp_ast::CppStmt> make_return_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppReturn(expression));}

std::shared_ptr<cpp_ast::CppStmt> make_expression_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppExprStmt(expression));}

std::shared_ptr<cpp_ast::CppStmt> make_break_cpp_statement() noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppExprStmt(make_identifier_cpp_expression(mlc::String("break"))));}

std::shared_ptr<cpp_ast::CppStmt> make_continue_cpp_statement() noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppExprStmt(make_identifier_cpp_expression(mlc::String("continue"))));}

std::shared_ptr<cpp_ast::CppStmt> make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> statements) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppBlock(statements));}

std::shared_ptr<cpp_ast::CppStmt> make_if_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> condition, std::shared_ptr<cpp_ast::CppStmt> then_branch, std::shared_ptr<cpp_ast::CppStmt> else_branch) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppIf(condition, then_branch, else_branch));}

std::shared_ptr<cpp_ast::CppStmt> make_while_cpp_statement(std::shared_ptr<cpp_ast::CppExpr> condition, std::shared_ptr<cpp_ast::CppStmt> body) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppWhile(condition, body));}

std::shared_ptr<cpp_ast::CppStmt> make_for_cpp_statement(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpr> range_expression, mlc::Array<std::shared_ptr<cpp_ast::CppStmt>> body) noexcept{return std::make_shared<cpp_ast::CppStmt>(cpp_ast::CppFor(variable_name, range_expression, body));}

std::shared_ptr<cpp_ast::CppDecl> make_fragment_cpp_declaration(mlc::String code) noexcept{return std::make_shared<cpp_ast::CppDecl>(cpp_ast::CppDeclFragment(code));}

} // namespace emit_helpers
