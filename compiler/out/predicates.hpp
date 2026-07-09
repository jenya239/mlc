#ifndef PREDICATES_HPP
#define PREDICATES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace predicates {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct Parser {mlc::Array<ast_tokens::Token> tokens;int position;mlc::String source_path;bool suppress_lambda_shorthand;mlc::Array<mlc::String> errors;bool has_unget_angle_close;};

template<typename Value>
struct ParseResult {Value value;predicates::Parser parser;};

using ExprResult = predicates::ParseResult<std::shared_ptr<ast::Expr>>;

using TypeResult = predicates::ParseResult<std::shared_ptr<ast::TypeExpr>>;

using PatternResult = predicates::ParseResult<std::shared_ptr<ast::Pattern>>;

using StmtResult = predicates::ParseResult<std::shared_ptr<ast::Stmt>>;

using StmtsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Stmt>>>;

using ExprsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Expr>>>;

using ArmsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::MatchArm>>>;

using VariantResult = predicates::ParseResult<std::shared_ptr<ast::TypeVariant>>;

using VariantsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeVariant>>>;

using ParamsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Param>>>;

using DeclResult = predicates::ParseResult<std::shared_ptr<ast::Decl>>;

using FieldValsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::FieldVal>>>;

using RecordLitPartsResult = predicates::ParseResult<mlc::Array<ast::RecordLitPart>>;

using PatternsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Pattern>>>;

using FieldDefsResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::FieldDef>>>;

using TypesResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::TypeExpr>>>;

using ParamResult = predicates::ParseResult<std::shared_ptr<ast::Param>>;

using NamesResult = predicates::ParseResult<mlc::Array<mlc::String>>;

struct TypeParamsValue {mlc::Array<mlc::String> params;mlc::Array<mlc::Array<mlc::String>> bounds;};

using TypeParamsResult = predicates::ParseResult<predicates::TypeParamsValue>;

using BoundsResult = predicates::ParseResult<mlc::Array<mlc::String>>;

using WhereClauseParseResult = predicates::ParseResult<mlc::Array<ast::WhereClauseBound>>;

using TraitBodyResult = predicates::ParseResult<mlc::Array<std::shared_ptr<ast::Decl>>>;

struct ProgramParseValue {ast::Program program;mlc::Array<mlc::String> errors;};

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

template<typename Value>
predicates::ParseResult<Value> parse_result(Value value, predicates::Parser parser) noexcept{return predicates::ParseResult<Value>{value, parser};}

predicates::ParseResult<mlc::Array<mlc::String>> bounds_parse_result(mlc::Array<mlc::String> bounds, predicates::Parser parser) noexcept;

predicates::TypeResult type_parse_result(std::shared_ptr<ast::TypeExpr> type_expression, predicates::Parser parser) noexcept;

predicates::PatternResult pattern_parse_result(std::shared_ptr<ast::Pattern> pattern, predicates::Parser parser) noexcept;

predicates::StmtResult statement_parse_result(std::shared_ptr<ast::Stmt> statement, predicates::Parser parser) noexcept;

predicates::VariantResult variant_parse_result(std::shared_ptr<ast::TypeVariant> variant, predicates::Parser parser) noexcept;

predicates::StmtsResult statements_parse_result(mlc::Array<std::shared_ptr<ast::Stmt>> statements, predicates::Parser parser) noexcept;

predicates::ExprsResult expressions_parse_result(mlc::Array<std::shared_ptr<ast::Expr>> expressions, predicates::Parser parser) noexcept;

predicates::ArmsResult arms_parse_result(mlc::Array<std::shared_ptr<ast::MatchArm>> arms, predicates::Parser parser) noexcept;

predicates::VariantsResult variants_parse_result(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, predicates::Parser parser) noexcept;

predicates::ParamsResult parameters_parse_result(mlc::Array<std::shared_ptr<ast::Param>> parameters, predicates::Parser parser) noexcept;

predicates::FieldValsResult field_values_parse_result(mlc::Array<std::shared_ptr<ast::FieldVal>> field_values, predicates::Parser parser) noexcept;

predicates::RecordLitPartsResult record_lit_parts_parse_result(mlc::Array<ast::RecordLitPart> lit_parts, predicates::Parser parser) noexcept;

predicates::PatternsResult patterns_parse_result(mlc::Array<std::shared_ptr<ast::Pattern>> patterns, predicates::Parser parser) noexcept;

predicates::FieldDefsResult field_definitions_parse_result(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, predicates::Parser parser) noexcept;

predicates::TypesResult types_parse_result(mlc::Array<std::shared_ptr<ast::TypeExpr>> types, predicates::Parser parser) noexcept;

predicates::DeclResult declaration_parse_result(std::shared_ptr<ast::Decl> declaration, predicates::Parser parser) noexcept;

predicates::WhereClauseParseResult where_clause_parse_result(mlc::Array<ast::WhereClauseBound> where_bounds, predicates::Parser parser) noexcept;

predicates::TraitBodyResult trait_body_parse_result(mlc::Array<std::shared_ptr<ast::Decl>> methods, predicates::Parser parser) noexcept;

predicates::ExprResult expression_parse_result(std::shared_ptr<ast::Expr> expression, predicates::Parser parser) noexcept;

predicates::TypeParamsResult type_params_parse_result(mlc::Array<mlc::String> params, mlc::Array<mlc::Array<mlc::String>> bounds, predicates::Parser parser) noexcept;

predicates::ProgramParseValue program_parse_result(ast::Program program, mlc::Array<mlc::String> errors) noexcept;

bool is_ctor_name(mlc::String name) noexcept;

bool TokenKind_is_fn(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_type(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_let(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_mut(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_const(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_if(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_unless(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_else(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_end(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_do(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_while(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_for(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_with(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_spawn(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_match(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_return(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_break(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_continue(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_true(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_false(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_extern(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_extend(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_where(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_import(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_as(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_from(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_eof(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_ident(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_int(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_str(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_template(ast_tokens::TokenKind self) noexcept;

mlc::Array<mlc::String> TokenKind_template_parts(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_equal(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_bar(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_pipe(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_colon(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_comma(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_dot(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_spread(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_arrow(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_fat_arrow(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_question(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lparen(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rparen(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lbrace(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rbrace(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_lbracket(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_rbracket(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_op(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_ident(ast_tokens::TokenKind self) noexcept;

int TokenKind_int_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_str_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_op_val(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_float(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_i64(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_u8(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_usize(ast_tokens::TokenKind self) noexcept;

bool TokenKind_is_char_lit(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_float_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_i64_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_u8_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_usize_val(ast_tokens::TokenKind self) noexcept;

mlc::String TokenKind_char_val(ast_tokens::TokenKind self) noexcept;

ast_tokens::TokenKind Parser_kind(predicates::Parser self) noexcept;

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept;

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept;

predicates::Parser Parser_record_parse_error(predicates::Parser self, mlc::String message) noexcept;

bool Parser_has_parse_errors(predicates::Parser self) noexcept;

predicates::Parser Parser_advance(predicates::Parser self) noexcept;

predicates::Parser Parser_advance_shift_right_as_angle_close(predicates::Parser self) noexcept;

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept;

bool Parser_at_eof(predicates::Parser self) noexcept;

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept;

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept;

int Parser_prev_line(predicates::Parser self) noexcept;

} // namespace predicates

#endif // PREDICATES_HPP
