#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"
#include "decls.hpp"

namespace symbols {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

mlc::HashMap<mlc::String, ast::Span> build_symbol_table(ast::Program program, mlc::String source_file) noexcept;

mlc::String find_identifier_at_position(mlc::String source_text, int line_zero_based, int column_zero_based) noexcept;

ast::Span resolve_definition_span(mlc::HashMap<mlc::String, ast::Span> symbol_table, mlc::String identifier_name) noexcept;

ast::Span resolve_definition_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept;

} // namespace symbols

#endif // SYMBOLS_HPP
