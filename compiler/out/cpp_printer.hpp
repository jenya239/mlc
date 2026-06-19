#ifndef CPP_PRINTER_HPP
#define CPP_PRINTER_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"

namespace cpp_printer {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

} // namespace cpp_printer

#endif // CPP_PRINTER_HPP
