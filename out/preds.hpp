#ifndef PREDS_HPP
#define PREDS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace preds {

struct Parser {mlc::Array<ast_tokens::Token> tokens;int pos;};
struct ExprResult {std::shared_ptr<ast::Expr> expr;Parser parser;};
struct TypeResult {std::shared_ptr<ast::TypeExpr> type_expr;Parser parser;};
struct PatResult {std::shared_ptr<ast::Pat> pat;Parser parser;};
struct StmtResult {std::shared_ptr<ast::Stmt> stmt;Parser parser;};
struct StmtsResult {mlc::Array<std::shared_ptr<ast::Stmt>> stmts;Parser parser;};
struct ExprsResult {mlc::Array<std::shared_ptr<ast::Expr>> exprs;Parser parser;};
struct ArmsResult {mlc::Array<std::shared_ptr<ast::MatchArm>> arms;Parser parser;};
struct VariantResult {std::shared_ptr<ast::TypeVariant> variant;Parser parser;};
struct VariantsResult {mlc::Array<std::shared_ptr<ast::TypeVariant>> variants;Parser parser;};
struct ParamsResult {mlc::Array<std::shared_ptr<ast::Param>> params;Parser parser;};
struct DeclResult {std::shared_ptr<ast::Decl> decl;Parser parser;};
struct FieldValsResult {mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals;Parser parser;};
struct PatsResult {mlc::Array<std::shared_ptr<ast::Pat>> pats;Parser parser;};
struct FieldDefsResult {mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs;Parser parser;};
struct TypesResult {mlc::Array<std::shared_ptr<ast::TypeExpr>> types;Parser parser;};
struct ParamResult {std::shared_ptr<ast::Param> param;Parser parser;};
struct NamesResult {mlc::Array<mlc::String> exprs;Parser parser;};
Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;
ast_tokens::TKind parser_kind(Parser parser) noexcept;
Parser parser_advance(Parser parser) noexcept;
Parser parser_advance_by(Parser parser, int count) noexcept;
bool parser_at_eof(Parser parser) noexcept;
Parser parser_skip_semi(Parser parser) noexcept;
bool is_keyword_fn(ast_tokens::TKind kind) noexcept;
bool is_keyword_type(ast_tokens::TKind kind) noexcept;
bool is_keyword_let(ast_tokens::TKind kind) noexcept;
bool is_keyword_mut(ast_tokens::TKind kind) noexcept;
bool is_keyword_const(ast_tokens::TKind kind) noexcept;
bool is_keyword_if(ast_tokens::TKind kind) noexcept;
bool is_keyword_unless(ast_tokens::TKind kind) noexcept;
bool is_keyword_else(ast_tokens::TKind kind) noexcept;
bool is_keyword_end(ast_tokens::TKind kind) noexcept;
bool is_keyword_do(ast_tokens::TKind kind) noexcept;
bool is_keyword_while(ast_tokens::TKind kind) noexcept;
bool is_keyword_for(ast_tokens::TKind kind) noexcept;
bool is_keyword_match(ast_tokens::TKind kind) noexcept;
bool is_keyword_return(ast_tokens::TKind kind) noexcept;
bool is_keyword_break(ast_tokens::TKind kind) noexcept;
bool is_keyword_continue(ast_tokens::TKind kind) noexcept;
bool is_keyword_true(ast_tokens::TKind kind) noexcept;
bool is_keyword_false(ast_tokens::TKind kind) noexcept;
bool is_keyword_extern(ast_tokens::TKind kind) noexcept;
bool is_keyword_extend(ast_tokens::TKind kind) noexcept;
bool is_ident(ast_tokens::TKind kind) noexcept;
bool is_int_literal(ast_tokens::TKind kind) noexcept;
bool is_str_literal(ast_tokens::TKind kind) noexcept;
bool is_equal(ast_tokens::TKind kind) noexcept;
bool is_bar(ast_tokens::TKind kind) noexcept;
bool is_pipe(ast_tokens::TKind kind) noexcept;
bool is_colon(ast_tokens::TKind kind) noexcept;
bool is_comma(ast_tokens::TKind kind) noexcept;
bool is_dot(ast_tokens::TKind kind) noexcept;
bool is_spread(ast_tokens::TKind kind) noexcept;
bool is_arrow(ast_tokens::TKind kind) noexcept;
bool is_fat_arrow(ast_tokens::TKind kind) noexcept;
bool is_question(ast_tokens::TKind kind) noexcept;
bool is_left_paren(ast_tokens::TKind kind) noexcept;
bool is_right_paren(ast_tokens::TKind kind) noexcept;
bool is_left_brace(ast_tokens::TKind kind) noexcept;
bool is_right_brace(ast_tokens::TKind kind) noexcept;
bool is_left_bracket(ast_tokens::TKind kind) noexcept;
bool is_right_bracket(ast_tokens::TKind kind) noexcept;
bool is_operator(ast_tokens::TKind kind) noexcept;
mlc::String get_ident(ast_tokens::TKind kind) noexcept;
int get_int_literal(ast_tokens::TKind kind) noexcept;
mlc::String get_str_literal(ast_tokens::TKind kind) noexcept;
mlc::String get_operator(ast_tokens::TKind kind) noexcept;
bool is_ctor_name(mlc::String name) noexcept;
bool TKind_is_fn(ast_tokens::TKind self) noexcept;
bool TKind_is_type(ast_tokens::TKind self) noexcept;
bool TKind_is_let(ast_tokens::TKind self) noexcept;
bool TKind_is_mut(ast_tokens::TKind self) noexcept;
bool TKind_is_const(ast_tokens::TKind self) noexcept;
bool TKind_is_if(ast_tokens::TKind self) noexcept;
bool TKind_is_unless(ast_tokens::TKind self) noexcept;
bool TKind_is_else(ast_tokens::TKind self) noexcept;
bool TKind_is_end(ast_tokens::TKind self) noexcept;
bool TKind_is_do(ast_tokens::TKind self) noexcept;
bool TKind_is_while(ast_tokens::TKind self) noexcept;
bool TKind_is_for(ast_tokens::TKind self) noexcept;
bool TKind_is_match(ast_tokens::TKind self) noexcept;
bool TKind_is_return(ast_tokens::TKind self) noexcept;
bool TKind_is_break(ast_tokens::TKind self) noexcept;
bool TKind_is_continue(ast_tokens::TKind self) noexcept;
bool TKind_is_true(ast_tokens::TKind self) noexcept;
bool TKind_is_false(ast_tokens::TKind self) noexcept;
bool TKind_is_extern(ast_tokens::TKind self) noexcept;
bool TKind_is_extend(ast_tokens::TKind self) noexcept;
bool TKind_is_import(ast_tokens::TKind self) noexcept;
bool TKind_is_from(ast_tokens::TKind self) noexcept;
bool TKind_is_eof(ast_tokens::TKind self) noexcept;
bool TKind_is_ident(ast_tokens::TKind self) noexcept;
bool TKind_is_int(ast_tokens::TKind self) noexcept;
bool TKind_is_str(ast_tokens::TKind self) noexcept;
bool TKind_is_equal(ast_tokens::TKind self) noexcept;
bool TKind_is_bar(ast_tokens::TKind self) noexcept;
bool TKind_is_pipe(ast_tokens::TKind self) noexcept;
bool TKind_is_colon(ast_tokens::TKind self) noexcept;
bool TKind_is_comma(ast_tokens::TKind self) noexcept;
bool TKind_is_dot(ast_tokens::TKind self) noexcept;
bool TKind_is_spread(ast_tokens::TKind self) noexcept;
bool TKind_is_arrow(ast_tokens::TKind self) noexcept;
bool TKind_is_fat_arrow(ast_tokens::TKind self) noexcept;
bool TKind_is_question(ast_tokens::TKind self) noexcept;
bool TKind_is_lparen(ast_tokens::TKind self) noexcept;
bool TKind_is_rparen(ast_tokens::TKind self) noexcept;
bool TKind_is_lbrace(ast_tokens::TKind self) noexcept;
bool TKind_is_rbrace(ast_tokens::TKind self) noexcept;
bool TKind_is_lbracket(ast_tokens::TKind self) noexcept;
bool TKind_is_rbracket(ast_tokens::TKind self) noexcept;
bool TKind_is_op(ast_tokens::TKind self) noexcept;
mlc::String TKind_ident(ast_tokens::TKind self) noexcept;
int TKind_int_val(ast_tokens::TKind self) noexcept;
mlc::String TKind_str_val(ast_tokens::TKind self) noexcept;
mlc::String TKind_op_val(ast_tokens::TKind self) noexcept;
ast_tokens::TKind Parser_kind(Parser self) noexcept;
Parser Parser_advance(Parser self) noexcept;
Parser Parser_advance_by(Parser self, int count) noexcept;
bool Parser_at_eof(Parser self) noexcept;
Parser Parser_skip_semi(Parser self) noexcept;

} // namespace preds

#endif // PREDS_HPP
