#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "ast.hpp"
namespace ast_printer {

mlc::Array<int> zero_based_indices(int count) noexcept;
mlc::String indent_text(int depth) noexcept;
mlc::String padding_spaces(int count) noexcept;
mlc::String escape_string_literal_content(mlc::String input) noexcept;
mlc::String print_string_literal(mlc::String value) noexcept;
mlc::String print_bool_literal(bool value) noexcept;
mlc::String print_unit_literal() noexcept;
mlc::String record_spread_text() noexcept;
mlc::String print_mlc_type(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::String print_mlc_parameter(std::shared_ptr<ast::Param> parameter) noexcept;
mlc::String print_mlc_parameters(mlc::Array<std::shared_ptr<ast::Param>> parameters) noexcept;
int max_field_name_length_from_values(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;
int max_field_name_length_from_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
int record_fields_count(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values) noexcept;
int record_lit_part_field_count(ast::RecordLitPart part) noexcept;
int record_lit_part_max_name_length(ast::RecordLitPart part) noexcept;
int max_field_name_length_in_lit_parts(mlc::Array<ast::RecordLitPart> lit_parts) noexcept;
int record_lit_parts_field_value_count(mlc::Array<ast::RecordLitPart> lit_parts) noexcept;
mlc::String print_field_value_inline(std::shared_ptr<ast::FieldVal> field_value, int depth) noexcept;
mlc::String print_field_values_inline(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept;
mlc::String print_field_value_aligned(std::shared_ptr<ast::FieldVal> field_value, int name_width, int depth) noexcept;
mlc::Array<mlc::String> append_field_values_inline(mlc::Array<mlc::String> segments, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int depth) noexcept;
mlc::Array<mlc::String> append_record_lit_part_inline(mlc::Array<mlc::String> segments, ast::RecordLitPart part, int depth) noexcept;
mlc::String print_record_lit_parts_inline(mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept;
mlc::Array<mlc::String> append_field_values_multiline(mlc::Array<mlc::String> lines, mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, int name_width, int depth) noexcept;
mlc::Array<mlc::String> append_record_lit_part_multiline(mlc::Array<mlc::String> lines, ast::RecordLitPart part, int name_width, int depth) noexcept;
mlc::String print_record_lit_parts_multiline(mlc::Array<ast::RecordLitPart> lit_parts, int name_width, int depth) noexcept;
mlc::String print_record_literal(mlc::String type_name, mlc::Array<ast::RecordLitPart> lit_parts, int depth) noexcept;
mlc::String print_record_update(mlc::String type_name, std::shared_ptr<ast::Expr> base, mlc::Array<std::shared_ptr<ast::FieldVal>> overrides, int depth) noexcept;
mlc::String print_type_fields_inline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::String print_type_fields_multiline(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int name_width, int depth) noexcept;
mlc::String print_type_record_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int depth) noexcept;
mlc::String format_do_end_block(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept;
mlc::String print_mlc_stmt_expression_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept;
mlc::String print_mlc_block_lines(mlc::Array<std::shared_ptr<ast::Stmt>> statements, std::shared_ptr<ast::Expr> result_expression, int depth) noexcept;
mlc::String print_mlc_let_stmt(mlc::String name, bool is_mutable, std::shared_ptr<ast::Expr> value, int depth) noexcept;
mlc::String print_mlc_type_decl(mlc::String name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String print_mlc_stmt_at_depth(std::shared_ptr<ast::Stmt> statement, int depth) noexcept;
mlc::String print_mlc_expr_at_depth(std::shared_ptr<ast::Expr> expression, int depth) noexcept;
mlc::String print_mlc_expr(std::shared_ptr<ast::Expr> expression) noexcept;
mlc::String print_mlc_function_body(std::shared_ptr<ast::Expr> body_expression, int depth) noexcept;
mlc::String print_mlc_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String print_mlc_program(ast::Program program) noexcept;

} // namespace ast_printer

#endif // AST_PRINTER_HPP
