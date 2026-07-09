#ifndef HASH_STMT_HELPERS_HPP
#define HASH_STMT_HELPERS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
namespace hash_stmt_helpers {

std::shared_ptr<cpp_ast::CppExpression> make_hash_call_expression(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept;
std::shared_ptr<cpp_ast::CppExpression> add_hash_combine_terms(std::shared_ptr<cpp_ast::CppExpression> left, std::shared_ptr<cpp_ast::CppExpression> right) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_hash_combine_right_side(std::shared_ptr<cpp_ast::CppExpression> access, mlc::String cpp_type) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_cpp_statement() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_cpp_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_cpp_statement() noexcept;

} // namespace hash_stmt_helpers

#endif // HASH_STMT_HELPERS_HPP
