#include "cpp_printer.hpp"

#include "cpp_ast.hpp"

namespace cpp_printer {

using namespace cpp_ast;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpression> expression) noexcept{return cpp_ast::printer_print_expr(expression);}

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStatement> statement) noexcept{return cpp_ast::printer_print_statement(statement);}

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return cpp_ast::printer_print_decl(declaration);}

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept{return cpp_ast::printer_print_file(file);}

mlc::String print_integer_literal(int integer_value) noexcept{return cpp_ast::printer_print_integer_literal(integer_value);}

mlc::String print_string_literal(mlc::String string_value) noexcept{return cpp_ast::printer_print_string_literal(string_value);}

mlc::String print_boolean_literal(bool boolean_value) noexcept{return cpp_ast::printer_print_boolean_literal(boolean_value);}

mlc::String print_identifier(mlc::String name) noexcept{return cpp_ast::printer_print_identifier(name);}

} // namespace cpp_printer
