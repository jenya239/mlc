#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "ast_tokens.hpp"
#include "lexer.hpp"
#include "decls.hpp"
namespace symbols {

ast::Span span_with_file(mlc::String source_file, ast::Span source_span) noexcept;
void symbol_table_set(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String name, mlc::String source_file, ast::Span source_span) noexcept;
void symbol_table_set_pattern_rest_name(mlc::HashMap<mlc::String, ast::Span>& table, mlc::String rest_name, mlc::String source_file, std::shared_ptr<ast::Pattern> pattern) noexcept;
void collect_first_pattern_alternative(mlc::Array<std::shared_ptr<ast::Pattern>> alternatives, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_pattern_list_definition_spans(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_expression_list_definition_spans(mlc::Array<std::shared_ptr<ast::Expr>> expressions, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_declaration_list_definition_spans(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_record_parts_definition_spans(mlc::Array<ast::RecordLitPart> parts, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_field_values_definition_spans(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_pattern_definition_spans(std::shared_ptr<ast::Pattern> pattern, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_statement_definition_spans(std::shared_ptr<ast::Stmt> statement, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_statements_definition_spans(mlc::Array<std::shared_ptr<ast::Stmt>> statements, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_match_arm_definition_spans(std::shared_ptr<ast::MatchArm> arm, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_expression_definition_spans(std::shared_ptr<ast::Expr> expression, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
void collect_declaration_definition_spans(std::shared_ptr<ast::Decl> declaration, mlc::String source_file, mlc::HashMap<mlc::String, ast::Span>& table) noexcept;
mlc::HashMap<mlc::String, ast::Span> build_symbol_table(ast::Program program, mlc::String source_file) noexcept;
mlc::String token_identifier_name(ast_tokens::Token token) noexcept;
mlc::String find_identifier_at_position(mlc::String source_text, int line_zero_based, int column_zero_based) noexcept;
ast::Span resolve_definition_span(mlc::HashMap<mlc::String, ast::Span> symbol_table, mlc::String identifier_name) noexcept;
ast::Span resolve_definition_in_source(mlc::String source_text, mlc::String source_file, int line_zero_based, int column_zero_based) noexcept;

} // namespace symbols

#endif // SYMBOLS_HPP
