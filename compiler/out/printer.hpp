#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_ast.hpp"
#include "cpp_ast.hpp"

namespace printer {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

mlc::String print_integer_literal(int integer_value) noexcept;

mlc::String print_string_literal(mlc::String string_value) noexcept;

mlc::String print_boolean_literal(bool boolean_value) noexcept;

mlc::String print_identifier(mlc::String name) noexcept;

mlc::String print_expr(std::shared_ptr<cpp_ast::CppExpr> expression) noexcept;

mlc::String print_statement(std::shared_ptr<cpp_ast::CppStmt> statement) noexcept;

mlc::String print_decl(std::shared_ptr<cpp_ast::CppDecl> declaration) noexcept;

mlc::String print_file(std::shared_ptr<cpp_ast::CppFile> file) noexcept;

} // namespace printer

#endif // PRINTER_HPP
