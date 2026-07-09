#ifndef DECLS_HPP
#define DECLS_HPP

#include "mlc.hpp"
#include <variant>
#include "predicates.hpp"
#include "ast_tokens.hpp"
#include "types.hpp"
#include "exprs.hpp"
#include "comma_separated.hpp"
#include "expect_close.hpp"
#include "ast.hpp"
namespace decls {

mlc::String type_expression_display(std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
mlc::String type_arguments_display(mlc::Array<std::shared_ptr<ast::TypeExpr>> type_arguments) noexcept;
predicates::Parser parser_expect_generic_close(predicates::Parser state, bool opened) noexcept;
predicates::Parser parser_expect_ident(predicates::Parser state, mlc::String context) noexcept;
bool decl_is_parse_skip(std::shared_ptr<ast::Decl> declaration) noexcept;
predicates::ParseResult<mlc::Array<mlc::String>> parse_trait_bound_ident_list(predicates::Parser parser_state) noexcept;
predicates::ParseResult<mlc::Array<mlc::String>> parse_one_param_bounds(predicates::Parser state) noexcept;
predicates::ParseResult<mlc::Array<ast::WhereClauseBound>> parse_where_clause_opt(predicates::Parser parser_state) noexcept;
bool string_list_contains(mlc::Array<mlc::String> haystack, mlc::String needle) noexcept;
mlc::Array<mlc::String> clone_string_row(mlc::Array<mlc::String> source) noexcept;
mlc::Array<mlc::Array<mlc::String>> clone_trait_bounds_matrix(mlc::Array<mlc::Array<mlc::String>> source) noexcept;
mlc::Array<mlc::String> union_trait_string_rows(mlc::Array<mlc::String> existing_row, mlc::Array<mlc::String> incoming_traits) noexcept;
int type_parameter_row_index(mlc::Array<mlc::String> type_parameter_names, mlc::String parameter_name) noexcept;
mlc::Array<mlc::Array<mlc::String>> replace_trait_bounds_row_at(mlc::Array<mlc::Array<mlc::String>> matrix, int row_index, mlc::Array<mlc::String> new_row) noexcept;
mlc::Array<mlc::Array<mlc::String>> merge_where_clause_trait_bounds(mlc::Array<mlc::String> type_parameter_names, mlc::Array<mlc::Array<mlc::String>> trait_bounds_rows, mlc::Array<ast::WhereClauseBound> where_entries) noexcept;
predicates::ParseResult<predicates::TypeParamsValue> parse_type_params_opt(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_rbrace(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_from(predicates::Parser parser) noexcept;
mlc::String import_path_from_parser(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_lbrace(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_comma(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_arrow(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_mut(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_bar(predicates::Parser parser) noexcept;
predicates::Parser parser_after_optional_private_keyword(predicates::Parser parser) noexcept;
ast::Span extern_keyword_span_for_parser(predicates::Parser parser, bool is_extern) noexcept;
predicates::Parser parser_after_optional_extern(predicates::Parser parser, bool is_extern) noexcept;
predicates::Parser trait_body_start_parser(predicates::Parser body_parser) noexcept;
predicates::Parser parser_record_error_if_eof_after_equal(predicates::Parser state) noexcept;
predicates::Parser parser_record_error_if_eof_expect_equal(predicates::Parser state) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_trait_method_body(predicates::Parser where_parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_import_decl(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_decl(predicates::Parser parser) noexcept;
predicates::Parser advance_past_optional_self_type(predicates::Parser parser_after_self) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_extern_no_self_params(predicates::Parser after_lparen) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_extern_rest_params(predicates::Parser after_lparen, bool leading_self) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_extern_method(predicates::Parser parser, mlc::String type_name) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_extend_method_rest_params(predicates::Parser rest_start) noexcept;
bool is_extern_ident_expr(std::shared_ptr<ast::Expr> expression) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_extend_method_body(predicates::Parser after_eq) noexcept;
predicates::Parser extend_method_rest_start(predicates::Parser after_lparen) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extend_method(predicates::Parser parser, mlc::String type_name) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_trait_decl(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_declaration(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extern_lib_declaration(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_extern_type_declaration(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Expr>> parse_extern_fn_body(predicates::Parser where_parser, ast::Span extern_keyword_span) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_function_declaration(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_tuple_parameter_binding_element(predicates::Parser binding_parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Pattern>> parse_tuple_parameter_pattern(ast::Span tuple_span, predicates::Parser parser_after_open_paren) noexcept;
std::shared_ptr<ast::Pattern> plain_identifier_parameter_pattern_marker() noexcept;
predicates::ParseResult<std::shared_ptr<ast::Param>> parse_param(predicates::Parser parser_for_parameter, int parameter_slot_index) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>> parse_params(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<mlc::String>> parse_derive_clause(predicates::Parser parser) noexcept;
bool type_decl_after_equal_is_sum_variant(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::Decl>> parse_type_decl(predicates::Parser parser) noexcept;
bool is_decl_start(ast_tokens::TokenKind kind) noexcept;
bool parse_variants_continue(predicates::Parser state) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeVariant>>> parse_variants(predicates::Parser parser) noexcept;
predicates::ParseResult<std::shared_ptr<ast::TypeVariant>> parse_variant(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>> parse_tuple_types(predicates::Parser parser) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Decl>>> parse_trait_body(predicates::Parser parser, mlc::String trait_name, mlc::Array<mlc::String> type_params) noexcept;
predicates::ParseResult<mlc::Array<std::shared_ptr<ast::FieldDef>>> parse_field_defs(predicates::Parser parser) noexcept;
ast::Program parse_program_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;
predicates::ProgramParseValue parse_program_with_errors(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;
ast::Program parse_program(mlc::Array<ast_tokens::Token> tokens) noexcept;

} // namespace decls

#endif // DECLS_HPP
