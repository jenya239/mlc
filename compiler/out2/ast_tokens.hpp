#ifndef AST_TOKENS_HPP
#define AST_TOKENS_HPP

#include "mlc.hpp"
#include <variant>

namespace ast_tokens {

struct Expr;
struct Stmt;
struct SExpr;
struct SStmt;

struct KFn {};struct KType {};struct KLet {};struct KMut {};struct KConst {};struct KReturn {};struct KBreak {};struct KContinue {};struct KIf {};struct KThen {};struct KElse {};struct KUnless {};struct KWhile {};struct KFor {};struct KIn {};struct KDo {};struct KEnd {};struct KMatch {};struct KImport {};struct KFrom {};struct KAs {};struct KExtern {};struct KExtend {};struct KWhere {};struct KTrue {};struct KFalse {};struct LInt {int field0;};struct LStr {mlc::String field0;};struct LTemplate {mlc::Array<mlc::String> field0;};struct Ident {mlc::String field0;};struct Arrow {};struct FatArrow {};struct Pipe {};struct Bar {};struct Equal {};struct Question {};struct Dot {};struct Spread {};struct LParen {};struct RParen {};struct LBrace {};struct RBrace {};struct LBracket {};struct RBracket {};struct Comma {};struct Semicolon {};struct Colon {};struct Op {mlc::String field0;};struct Eof {};using TKind = std::variant<KFn, KType, KLet, KMut, KConst, KReturn, KBreak, KContinue, KIf, KThen, KElse, KUnless, KWhile, KFor, KIn, KDo, KEnd, KMatch, KImport, KFrom, KAs, KExtern, KExtend, KWhere, KTrue, KFalse, LInt, LStr, LTemplate, Ident, Arrow, FatArrow, Pipe, Bar, Equal, Question, Dot, Spread, LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon, Op, Eof>;

struct Token {ast_tokens::TKind kind;int line;int col;};

struct LexOut {mlc::Array<ast_tokens::Token> tokens;mlc::Array<mlc::String> errors;};

bool LexOut_has_errors(ast_tokens::LexOut self) noexcept;

ast_tokens::TKind Token_kind_value(ast_tokens::Token self) noexcept;

int Token_line_number(ast_tokens::Token self) noexcept;

int Token_column(ast_tokens::Token self) noexcept;

} // namespace ast_tokens

#endif // AST_TOKENS_HPP
