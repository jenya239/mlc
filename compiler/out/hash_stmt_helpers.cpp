#define main mlc_user_main
#include "hash_stmt_helpers.hpp"

#include "cpp_ast.hpp"
#include "emit_helpers.hpp"

namespace hash_stmt_helpers {

using namespace cpp_ast;
using namespace emit_helpers;

std::shared_ptr<cpp_ast::CppExpression> make_hash_call_expression(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCall{std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppAggregateInit{((mlc::String("std::hash<", 10) + cpp_type) + mlc::String(">", 1)), {}}), mlc::Array<std::shared_ptr<cpp_ast::CppExpression>>{access}});
}
std::shared_ptr<cpp_ast::CppExpression> add_hash_combine_terms(std::shared_ptr<cpp_ast::CppExpression> left, std::shared_ptr<cpp_ast::CppExpression> right) noexcept{
  return std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("+", 1), left, right});
}
std::shared_ptr<cpp_ast::CppExpression> make_hash_combine_right_side(std::shared_ptr<cpp_ast::CppExpression> access, mlc::String cpp_type) noexcept{
  auto hash_call = make_hash_call_expression(cpp_type, access);
  auto hash_identifier = emit_helpers::make_identifier_cpp_expression(mlc::String("h", 1));
  return add_hash_combine_terms(add_hash_combine_terms(add_hash_combine_terms(hash_call, emit_helpers::make_identifier_cpp_expression(mlc::String("0x9e3779b97f4a7c15ULL", 21))), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("<<", 2), hash_identifier, emit_helpers::make_integer_cpp_expression(6)})), std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String(">>", 2), hash_identifier, emit_helpers::make_integer_cpp_expression(2)}));
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_cpp_statement() noexcept{
  return emit_helpers::make_variable_cpp_statement(mlc::String("h", 1), emit_helpers::make_size_t_type(), emit_helpers::make_identifier_cpp_expression(mlc::String("1469598103934665603ULL", 22)));
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_cpp_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept{
  return emit_helpers::make_expression_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppBinary{mlc::String("^=", 2), emit_helpers::make_identifier_cpp_expression(mlc::String("h", 1)), make_hash_combine_right_side(access, cpp_type)}));
}
std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_cpp_statement() noexcept{
  return emit_helpers::make_return_cpp_statement(std::make_shared<cpp_ast::CppExpression>(cpp_ast::CppCast{cpp_ast::CppCastStatic{}, emit_helpers::make_size_t_type(), emit_helpers::make_identifier_cpp_expression(mlc::String("1469598103934665603ULL", 22))}));
}

} // namespace hash_stmt_helpers
