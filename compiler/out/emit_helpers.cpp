#define main mlc_user_main
#include "emit_helpers.hpp"

#include "cpp_ast.hpp"

namespace emit_helpers {

using namespace cpp_ast;

std::shared_ptr<cpp_ast::CppExpression> make_integer_cpp_expression(int integer_value) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppInt{integer_value});
}
std::shared_ptr<cpp_ast::CppExpression> make_string_cpp_expression(mlc::String string_value) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppStr{string_value});
}
std::shared_ptr<cpp_ast::CppExpression> make_boolean_cpp_expression(bool boolean_value) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBool{boolean_value});
}
std::shared_ptr<cpp_ast::CppExpression> make_identifier_cpp_expression(mlc::String name) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppIdent{name});
}
std::shared_ptr<cpp_ast::CppStatement> make_constexpr_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppConstexprAutoDecl{name, initializer});
}
std::shared_ptr<cpp_ast::CppStatement> make_fragment_cpp_statement(mlc::String code) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppStatementFragment{code});
}
std::shared_ptr<cpp_ast::CppStatement> make_auto_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppAutoDecl{name, initializer});
}
std::shared_ptr<cpp_ast::CppStatement> make_return_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppReturn{expression});
}
std::shared_ptr<cpp_ast::CppStatement> make_expression_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppExpressionStatement{expression});
}
std::shared_ptr<cpp_ast::CppStatement> make_variable_cpp_statement(mlc::String name, std::shared_ptr<cpp_ast::CppType> type_node, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppVarDecl{name, type_node, initializer});
}
std::shared_ptr<cpp_ast::CppType> make_size_t_type() noexcept{
  return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("size_t", 6)});
}
std::shared_ptr<cpp_ast::CppStatement> make_break_cpp_statement() noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppExpressionStatement{make_identifier_cpp_expression(mlc::String("break", 5))});
}
std::shared_ptr<cpp_ast::CppStatement> make_continue_cpp_statement() noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppExpressionStatement{make_identifier_cpp_expression(mlc::String("continue", 8))});
}
std::shared_ptr<cpp_ast::CppStatement> make_block_cpp_statement(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppBlock{statements});
}
std::shared_ptr<cpp_ast::CppStatement> make_if_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> condition, std::shared_ptr<cpp_ast::CppStatement> then_branch, std::shared_ptr<cpp_ast::CppStatement> else_branch) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppIf{condition, then_branch, else_branch});
}
std::shared_ptr<cpp_ast::CppStatement> make_while_cpp_statement(std::shared_ptr<cpp_ast::CppExpression> condition, std::shared_ptr<cpp_ast::CppStatement> body) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppWhile{condition, body});
}
std::shared_ptr<cpp_ast::CppStatement> make_for_cpp_statement(mlc::String variable_name, std::shared_ptr<cpp_ast::CppExpression> range_expression, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppFor{variable_name, range_expression, body});
}
std::shared_ptr<cpp_ast::CppStatement> make_structured_binding_cpp_statement(mlc::Array<mlc::String> binding_names, std::shared_ptr<cpp_ast::CppExpression> initializer) noexcept{
  return std::make_shared<cpp_ast::CppStatement>(cpp_ast::CppStructuredBinding{binding_names, initializer});
}
std::shared_ptr<cpp_ast::CppDeclaration> make_fragment_cpp_declaration(mlc::String code) noexcept{
  return std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationFragment{code});
}

} // namespace emit_helpers
