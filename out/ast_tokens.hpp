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
struct KWith;
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
struct LTemplate;
struct Ident;
struct LFloat;
struct LI64;
struct LU8;
struct LUsize;
struct LChar;
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
using TKind = std::variant<KFn, KType, KLet, KMut, KConst, KReturn, KBreak, KContinue, KIf, KThen, KElse, KUnless, KWhile, KFor, KIn, KDo, KEnd, KMatch, KWith, KImport, KFrom, KAs, KExtern, KExtend, KWhere, KTrue, KFalse, LInt, LStr, LTemplate, Ident, LFloat, LI64, LU8, LUsize, LChar, Arrow, FatArrow, Pipe, Bar, Equal, Question, Dot, Spread, LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon, Op, Eof>;
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
struct KWith {};
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
struct LTemplate {mlc::Array<mlc::String> field0;};
struct Ident {mlc::String field0;};
struct LFloat {mlc::String field0;};
struct LI64 {mlc::String field0;};
struct LU8 {mlc::String field0;};
struct LUsize {mlc::String field0;};
struct LChar {mlc::String field0;};
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
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;

} // namespace ast_tokens

#endif // AST_TOKENS_HPP
