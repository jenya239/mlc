#ifndef PRINT_HPP
#define PRINT_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"

namespace print {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String print_cpp_type(std::shared_ptr<cpp_ast::CppType> type_node) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

} // namespace print

#endif // PRINT_HPP
