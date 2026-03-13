#ifndef PREDS_HPP
#define PREDS_HPP

#include "mlc.hpp"
#include <variant>

#include "ast_tokens.hpp"
#include "ast.hpp"

namespace preds {

struct Expr;
struct Stmt;

struct Parser {mlc::Array<ast_tokens::Token> tokens;int pos;};

struct ExprResult {std::shared_ptr<ast::Expr> expr;preds::Parser parser;};

struct TypeResult {std::shared_ptr<ast::TypeExpr> type_expr;preds::Parser parser;};

struct PatResult {std::shared_ptr<ast::Pat> pat;preds::Parser parser;};

struct StmtResult {std::shared_ptr<ast::Stmt> stmt;preds::Parser parser;};

struct StmtsResult {mlc::Array<std::shared_ptr<ast::Stmt>> stmts;preds::Parser parser;};

struct ExprsResult {mlc::Array<std::shared_ptr<ast::Expr>> exprs;preds::Parser parser;};

struct ArmsResult {mlc::Array<std::shared_ptr<ast::MatchArm>> arms;preds::Parser parser;};

struct VariantResult {std::shared_ptr<ast::TypeVariant> variant;preds::Parser parser;};

struct VariantsResult {mlc::Array<std::shared_ptr<ast::TypeVariant>> variants;preds::Parser parser;};

struct ParamsResult {mlc::Array<std::shared_ptr<ast::Param>> params;preds::Parser parser;};

struct DeclResult {std::shared_ptr<ast::Decl> decl;preds::Parser parser;};

struct FieldValsResult {mlc::Array<std::shared_ptr<ast::FieldVal>> field_vals;preds::Parser parser;};

struct PatsResult {mlc::Array<std::shared_ptr<ast::Pat>> pats;preds::Parser parser;};

struct FieldDefsResult {mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs;preds::Parser parser;};

struct TypesResult {mlc::Array<std::shared_ptr<ast::TypeExpr>> types;preds::Parser parser;};

struct ParamResult {std::shared_ptr<ast::Param> param;preds::Parser parser;};

struct NamesResult {mlc::Array<mlc::String> exprs;preds::Parser parser;};

preds::Parser parser_new(mlc::Array<ast_tokens::Token> tokens) noexcept;

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

ast_tokens::TKind Parser_kind(preds::Parser self) noexcept;

preds::Parser Parser_advance(preds::Parser self) noexcept;

preds::Parser Parser_advance_by(preds::Parser self, int count) noexcept;

bool Parser_at_eof(preds::Parser self) noexcept;

preds::Parser Parser_skip_semi(preds::Parser self) noexcept;

} // namespace preds

#endif // PREDS_HPP
