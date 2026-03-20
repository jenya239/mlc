#ifndef AST_TOKENS_HPP
#define AST_TOKENS_HPP

#include "mlc.hpp"
#include <variant>


namespace ast_tokens {

struct KFn;
struct KType;
struct KLet;
struct KMut;
struct KConst;
struct KReturn;
struct KBreak;
struct KContinue;
struct KIf;
struct KThen;
struct KElse;
struct KUnless;
struct KWhile;
struct KFor;
struct KIn;
struct KDo;
struct KEnd;
struct KMatch;
struct KImport;
struct KFrom;
struct KAs;
struct KExtern;
struct KExtend;
struct KWhere;
struct KTrue;
struct KFalse;
struct LInt;
struct LStr;
struct LFStr;
struct Ident;
struct Arrow;
struct FatArrow;
struct Pipe;
struct Bar;
struct Equal;
struct Question;
struct Dot;
struct Spread;
struct LParen;
struct RParen;
struct LBrace;
struct RBrace;
struct LBracket;
struct RBracket;
struct Comma;
struct Semicolon;
struct Colon;
struct Op;
struct Eof;
using TKind = std::variant<KFn, KType, KLet, KMut, KConst, KReturn, KBreak, KContinue, KIf, KThen, KElse, KUnless, KWhile, KFor, KIn, KDo, KEnd, KMatch, KImport, KFrom, KAs, KExtern, KExtend, KWhere, KTrue, KFalse, LInt, LStr, LFStr, Ident, Arrow, FatArrow, Pipe, Bar, Equal, Question, Dot, Spread, LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon, Op, Eof>;
struct KFn {};
struct KType {};
struct KLet {};
struct KMut {};
struct KConst {};
struct KReturn {};
struct KBreak {};
struct KContinue {};
struct KIf {};
struct KThen {};
struct KElse {};
struct KUnless {};
struct KWhile {};
struct KFor {};
struct KIn {};
struct KDo {};
struct KEnd {};
struct KMatch {};
struct KImport {};
struct KFrom {};
struct KAs {};
struct KExtern {};
struct KExtend {};
struct KWhere {};
struct KTrue {};
struct KFalse {};
struct LInt {int field0;};
struct LStr {mlc::String field0;};
struct LFStr {mlc::Array<mlc::String> field0;};
struct Ident {mlc::String field0;};
struct Arrow {};
struct FatArrow {};
struct Pipe {};
struct Bar {};
struct Equal {};
struct Question {};
struct Dot {};
struct Spread {};
struct LParen {};
struct RParen {};
struct LBrace {};
struct RBrace {};
struct LBracket {};
struct RBracket {};
struct Comma {};
struct Semicolon {};
struct Colon {};
struct Op {mlc::String field0;};
struct Eof {};
struct Token {TKind kind;int line;int col;};
struct LexOut {mlc::Array<Token> tokens;mlc::Array<mlc::String> errors;};
void LexOut_has_errors(LexOut self) noexcept;
void Token_kind_value(Token self) noexcept;

} // namespace ast_tokens

#endif // AST_TOKENS_HPP
