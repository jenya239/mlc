#ifndef PREDICATES_HPP
#define PREDICATES_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace predicates {

struct RecordLitPart;
struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;
struct CppStmt;
struct CppExpr;

struct Parser {mlc::Array<ast_tokens::Token> tokens;int pos;mlc::String source_path;bool suppress_lambda_shorthand;};

struct ExprResult {std::shared_ptr<ast::Expr> expr;predicates::Parser parser;};

struct TypeResult {std::shared_ptr<ast::TypeExpr> type_expr;predicates::Parser parser;};

struct PatResult {std::shared_ptr<ast::Pat> pat;predicates::Parser parser;};

struct StmtResult {std::shared_ptr<ast::Stmt> stmt;predicates::Parser parser;};

struct StmtsResult {mlc::Array<std::shared_ptr<ast::Stmt>> stmts;predicates::Parser parser;};

struct ExprsResult {mlc::Array<std::shared_ptr<ast::Expr>> exprs;predicates::Parser parser;};

struct ArmsResult {mlc::Array<std::shared_ptr<ast::MatchArm>> arms;predicates::Parser parser;};

struct VariantResult {std::shared_ptr<ast::TypeVariant> variant;predicates::Parser parser;};

struct VariantsResult {mlc::Array<std::shared_ptr<ast::TypeVariant>> variants;predicates::Parser parser;};

struct ParamsResult {mlc::Array<std::shared_ptr<ast::Param>> params;predicates::Parser parser;};

struct DeclResult {std::shared_ptr<ast::Decl> decl;predicates::Parser parser;};

struct FieldValsResult {mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals;predicates::Parser parser;};

struct RecordLitPartsResult {mlc::Array<ast::RecordLitPart> lit_parts;predicates::Parser parser;};

struct PatsResult {mlc::Array<std::shared_ptr<ast::Pat>> pats;predicates::Parser parser;};

struct FieldDefsResult {mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs;predicates::Parser parser;};

struct TypesResult {mlc::Array<std::shared_ptr<ast::TypeExpr>> types;predicates::Parser parser;};

struct ParamResult {std::shared_ptr<ast::Param> param;predicates::Parser parser;};

struct NamesResult {mlc::Array<mlc::String> exprs;predicates::Parser parser;};

struct TypeParamsResult {mlc::Array<mlc::String> params;mlc::Array<mlc::Array<mlc::String>> bounds;predicates::Parser parser;};

struct BoundsResult {mlc::Array<mlc::String> bounds;predicates::Parser parser;};

struct WhereClauseParseResult {mlc::Array<ast::WhereClauseBound> where_bounds;predicates::Parser parser;};

struct TraitBodyResult {mlc::Array<std::shared_ptr<ast::Decl>> methods;predicates::Parser parser;};

predicates::Parser parser_new_with_source_path(mlc::Array<ast_tokens::Token> tokens, mlc::String source_path) noexcept;

predicates::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

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

bool TKind_is_with(ast_tokens::TKind self) noexcept;

bool TKind_is_match(ast_tokens::TKind self) noexcept;

bool TKind_is_return(ast_tokens::TKind self) noexcept;

bool TKind_is_break(ast_tokens::TKind self) noexcept;

bool TKind_is_continue(ast_tokens::TKind self) noexcept;

bool TKind_is_true(ast_tokens::TKind self) noexcept;

bool TKind_is_false(ast_tokens::TKind self) noexcept;

bool TKind_is_extern(ast_tokens::TKind self) noexcept;

bool TKind_is_extend(ast_tokens::TKind self) noexcept;

bool TKind_is_where(ast_tokens::TKind self) noexcept;

bool TKind_is_import(ast_tokens::TKind self) noexcept;

bool TKind_is_as(ast_tokens::TKind self) noexcept;

bool TKind_is_from(ast_tokens::TKind self) noexcept;

bool TKind_is_eof(ast_tokens::TKind self) noexcept;

bool TKind_is_ident(ast_tokens::TKind self) noexcept;

bool TKind_is_int(ast_tokens::TKind self) noexcept;

bool TKind_is_str(ast_tokens::TKind self) noexcept;

bool TKind_is_template(ast_tokens::TKind self) noexcept;

mlc::Array<mlc::String> TKind_template_parts(ast_tokens::TKind self) noexcept;

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

bool TKind_is_float(ast_tokens::TKind self) noexcept;

bool TKind_is_i64(ast_tokens::TKind self) noexcept;

bool TKind_is_u8(ast_tokens::TKind self) noexcept;

bool TKind_is_usize(ast_tokens::TKind self) noexcept;

bool TKind_is_char_lit(ast_tokens::TKind self) noexcept;

mlc::String TKind_float_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_i64_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_u8_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_usize_val(ast_tokens::TKind self) noexcept;

mlc::String TKind_char_val(ast_tokens::TKind self) noexcept;

ast_tokens::TKind Parser_kind(predicates::Parser self) noexcept;

bool Parser_lambda_shorthand_suppression_active(predicates::Parser self) noexcept;

predicates::Parser Parser_with_lambda_shorthand_suppressed(predicates::Parser self, bool value) noexcept;

predicates::Parser Parser_advance(predicates::Parser self) noexcept;

predicates::Parser Parser_advance_by(predicates::Parser self, int count) noexcept;

bool Parser_at_eof(predicates::Parser self) noexcept;

predicates::Parser Parser_skip_semi(predicates::Parser self) noexcept;

ast::Span Parser_span_at_cursor(predicates::Parser self) noexcept;

int Parser_prev_line(predicates::Parser self) noexcept;

} // namespace predicates

#endif // PREDICATES_HPP
