#ifndef CPP_TOKENS_HPP
#define CPP_TOKENS_HPP

#include "mlc.hpp"
#include <variant>

namespace cpp_tokens {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct CKAuto {};struct CKConst {};struct CKConstexpr {};struct CKIf {};struct CKElse {};struct CKWhile {};struct CKFor {};struct CKReturn {};struct CKBreak {};struct CKStruct {};struct CKUsing {};struct CKNoexcept {};struct CKInline {};struct CKStatic {};struct CKVoid {};struct CKInt {};struct CKBool {};struct CKChar {};struct CKTrue {};struct CKFalse {};struct CKNullptr {};struct CKTemplate {};struct CKTypename {};struct CKNamespace {};struct CKNew {};struct CKDelete {};struct CKOperator {};struct CKClass {};struct CKEnum {};struct CKTypedef {};struct CKExtern {};struct CKSigned {};struct CKUnsigned {};struct CKLong {};struct CKShort {};struct CKFloat {};struct CKDouble {};struct CKVirtual {};struct CKOverride {};struct CKFinal {};struct CKPublic {};struct CKPrivate {};struct CKProtected {};struct CKExplicit {};struct CKMutable {};struct CKVolatile {};struct CKFriend {};struct CIdent {mlc::String field0;};struct CLInt {int field0;};struct CLStr {mlc::String field0;};struct CArrow {};struct CScope {};struct CEq {};struct CNe {};struct CLe {};struct CGe {};struct CAnd {};struct COr {};struct CInc {};struct CDec {};struct CLShift {};struct CRShift {};struct CLParen {};struct CRParen {};struct CLBrace {};struct CRBrace {};struct CLBracket {};struct CRBracket {};struct CLAngle {};struct CRAngle {};struct CHash {};struct COp {mlc::String field0;};struct CEof {};using CppTokenKind = std::variant<CKAuto, CKConst, CKConstexpr, CKIf, CKElse, CKWhile, CKFor, CKReturn, CKBreak, CKStruct, CKUsing, CKNoexcept, CKInline, CKStatic, CKVoid, CKInt, CKBool, CKChar, CKTrue, CKFalse, CKNullptr, CKTemplate, CKTypename, CKNamespace, CKNew, CKDelete, CKOperator, CKClass, CKEnum, CKTypedef, CKExtern, CKSigned, CKUnsigned, CKLong, CKShort, CKFloat, CKDouble, CKVirtual, CKOverride, CKFinal, CKPublic, CKPrivate, CKProtected, CKExplicit, CKMutable, CKVolatile, CKFriend, CIdent, CLInt, CLStr, CArrow, CScope, CEq, CNe, CLe, CGe, CAnd, COr, CInc, CDec, CLShift, CRShift, CLParen, CRParen, CLBrace, CRBrace, CLBracket, CRBracket, CLAngle, CRAngle, CHash, COp, CEof>;

struct CppToken {cpp_tokens::CppTokenKind kind;int line;int column;};

bool is_cpp_reserved_keyword(mlc::String word) noexcept;

cpp_tokens::CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept;

cpp_tokens::CppTokenKind cpp_keyword_kind(mlc::String word) noexcept;

mlc::String cpp_token_lexeme(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_tokens_to_source(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_token_kinds_equal(mlc::Array<cpp_tokens::CppToken> first, mlc::Array<cpp_tokens::CppToken> second) noexcept;

} // namespace cpp_tokens

#endif // CPP_TOKENS_HPP
