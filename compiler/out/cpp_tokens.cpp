#define main mlc_user_main
#include "cpp_tokens.hpp"

namespace cpp_tokens {

CppToken cpp_token_make(CppTokenKind kind, int line, int column) noexcept{
  return CppToken{kind, line, column, mlc::String("", 0), mlc::String("", 0)};
}
CppToken cpp_token_with_trivia(CppToken token, mlc::String leading_trivia, mlc::String trailing_trivia) noexcept{
  return CppToken{token.kind, token.line, token.column, leading_trivia, trailing_trivia};
}
mlc::Array<CppKeywordEntry> cpp_keyword_entries() noexcept{
  auto entries = mlc::Array<CppKeywordEntry>{CppKeywordEntry{mlc::String("auto", 4), CKAuto{}}, CppKeywordEntry{mlc::String("const", 5), CKConst{}}, CppKeywordEntry{mlc::String("constexpr", 9), CKConstexpr{}}, CppKeywordEntry{mlc::String("if", 2), CKIf{}}, CppKeywordEntry{mlc::String("else", 4), CKElse{}}, CppKeywordEntry{mlc::String("while", 5), CKWhile{}}, CppKeywordEntry{mlc::String("for", 3), CKFor{}}, CppKeywordEntry{mlc::String("return", 6), CKReturn{}}, CppKeywordEntry{mlc::String("break", 5), CKBreak{}}, CppKeywordEntry{mlc::String("struct", 6), CKStruct{}}, CppKeywordEntry{mlc::String("using", 5), CKUsing{}}, CppKeywordEntry{mlc::String("noexcept", 8), CKNoexcept{}}, CppKeywordEntry{mlc::String("inline", 6), CKInline{}}, CppKeywordEntry{mlc::String("static", 6), CKStatic{}}, CppKeywordEntry{mlc::String("void", 4), CKVoid{}}, CppKeywordEntry{mlc::String("int", 3), CKInt{}}, CppKeywordEntry{mlc::String("bool", 4), CKBool{}}, CppKeywordEntry{mlc::String("char", 4), CKChar{}}, CppKeywordEntry{mlc::String("true", 4), CKTrue{}}, CppKeywordEntry{mlc::String("false", 5), CKFalse{}}, CppKeywordEntry{mlc::String("nullptr", 7), CKNullptr{}}, CppKeywordEntry{mlc::String("template", 8), CKTemplate{}}, CppKeywordEntry{mlc::String("typename", 8), CKTypename{}}, CppKeywordEntry{mlc::String("namespace", 9), CKNamespace{}}, CppKeywordEntry{mlc::String("new", 3), CKNew{}}, CppKeywordEntry{mlc::String("delete", 6), CKDelete{}}, CppKeywordEntry{mlc::String("operator", 8), CKOperator{}}, CppKeywordEntry{mlc::String("class", 5), CKClass{}}, CppKeywordEntry{mlc::String("enum", 4), CKEnum{}}, CppKeywordEntry{mlc::String("typedef", 7), CKTypedef{}}, CppKeywordEntry{mlc::String("extern", 6), CKExtern{}}, CppKeywordEntry{mlc::String("signed", 6), CKSigned{}}, CppKeywordEntry{mlc::String("unsigned", 8), CKUnsigned{}}, CppKeywordEntry{mlc::String("long", 4), CKLong{}}, CppKeywordEntry{mlc::String("short", 5), CKShort{}}, CppKeywordEntry{mlc::String("float", 5), CKFloat{}}, CppKeywordEntry{mlc::String("double", 6), CKDouble{}}, CppKeywordEntry{mlc::String("virtual", 7), CKVirtual{}}, CppKeywordEntry{mlc::String("override", 8), CKOverride{}}, CppKeywordEntry{mlc::String("final", 5), CKFinal{}}, CppKeywordEntry{mlc::String("public", 6), CKPublic{}}, CppKeywordEntry{mlc::String("private", 7), CKPrivate{}}, CppKeywordEntry{mlc::String("protected", 9), CKProtected{}}, CppKeywordEntry{mlc::String("explicit", 8), CKExplicit{}}, CppKeywordEntry{mlc::String("mutable", 7), CKMutable{}}, CppKeywordEntry{mlc::String("volatile", 8), CKVolatile{}}, CppKeywordEntry{mlc::String("friend", 6), CKFriend{}}};
  return entries;
}
bool is_cpp_reserved_keyword(mlc::String word) noexcept{
  return cpp_keyword_entries().any([=](CppKeywordEntry entry) mutable { return (entry.name == word); });
}
CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept{
  auto entries = cpp_keyword_entries();
  auto index = 0;
  while ((index < entries.length()))   {
    if ((entries[index].name == word))     {
      return entries[index].kind;
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return CKAuto{};
}
CppTokenKind cpp_keyword_kind(mlc::String word) noexcept{
  if (is_cpp_reserved_keyword(word))   {
    return reserved_cpp_keyword_kind(word);
  } else   {
    return CIdent{word};
  }
}
mlc::String cpp_char_literal_lexeme(mlc::String character) noexcept{
  auto escaped = ((character == mlc::String("\\", 1)) ? (mlc::String("\\\\", 2)) : (((character == mlc::String("'", 1)) ? (mlc::String("\\'", 2)) : (((character == mlc::String("\n", 1)) ? (mlc::String("\\n", 2)) : (((character == mlc::String("\r", 1)) ? (mlc::String("\\r", 2)) : (((character == mlc::String("\t", 1)) ? (mlc::String("\\t", 2)) : (((character == mlc::String("\0", 1)) ? (mlc::String("\\0", 2)) : (character))))))))))));
  return ((mlc::String("'", 1) + escaped) + mlc::String("'", 1));
}
mlc::String cpp_token_lexeme(CppTokenKind kind) noexcept{
  return std::visit(overloaded{[&](const CKAuto& cKAuto) { return mlc::String("auto", 4); },
[&](const CKConst& cKConst) { return mlc::String("const", 5); },
[&](const CKConstexpr& cKConstexpr) { return mlc::String("constexpr", 9); },
[&](const CKIf& cKIf) { return mlc::String("if", 2); },
[&](const CKElse& cKElse) { return mlc::String("else", 4); },
[&](const CKWhile& cKWhile) { return mlc::String("while", 5); },
[&](const CKFor& cKFor) { return mlc::String("for", 3); },
[&](const CKReturn& cKReturn) { return mlc::String("return", 6); },
[&](const CKBreak& cKBreak) { return mlc::String("break", 5); },
[&](const CKStruct& cKStruct) { return mlc::String("struct", 6); },
[&](const CKUsing& cKUsing) { return mlc::String("using", 5); },
[&](const CKNoexcept& cKNoexcept) { return mlc::String("noexcept", 8); },
[&](const CKInline& cKInline) { return mlc::String("inline", 6); },
[&](const CKStatic& cKStatic) { return mlc::String("static", 6); },
[&](const CKVoid& cKVoid) { return mlc::String("void", 4); },
[&](const CKInt& cKInt) { return mlc::String("int", 3); },
[&](const CKBool& cKBool) { return mlc::String("bool", 4); },
[&](const CKChar& cKChar) { return mlc::String("char", 4); },
[&](const CKTrue& cKTrue) { return mlc::String("true", 4); },
[&](const CKFalse& cKFalse) { return mlc::String("false", 5); },
[&](const CKNullptr& cKNullptr) { return mlc::String("nullptr", 7); },
[&](const CKTemplate& cKTemplate) { return mlc::String("template", 8); },
[&](const CKTypename& cKTypename) { return mlc::String("typename", 8); },
[&](const CKNamespace& cKNamespace) { return mlc::String("namespace", 9); },
[&](const CKNew& cKNew) { return mlc::String("new", 3); },
[&](const CKDelete& cKDelete) { return mlc::String("delete", 6); },
[&](const CKOperator& cKOperator) { return mlc::String("operator", 8); },
[&](const CKClass& cKClass) { return mlc::String("class", 5); },
[&](const CKEnum& cKEnum) { return mlc::String("enum", 4); },
[&](const CKTypedef& cKTypedef) { return mlc::String("typedef", 7); },
[&](const CKExtern& cKExtern) { return mlc::String("extern", 6); },
[&](const CKSigned& cKSigned) { return mlc::String("signed", 6); },
[&](const CKUnsigned& cKUnsigned) { return mlc::String("unsigned", 8); },
[&](const CKLong& cKLong) { return mlc::String("long", 4); },
[&](const CKShort& cKShort) { return mlc::String("short", 5); },
[&](const CKFloat& cKFloat) { return mlc::String("float", 5); },
[&](const CKDouble& cKDouble) { return mlc::String("double", 6); },
[&](const CKVirtual& cKVirtual) { return mlc::String("virtual", 7); },
[&](const CKOverride& cKOverride) { return mlc::String("override", 8); },
[&](const CKFinal& cKFinal) { return mlc::String("final", 5); },
[&](const CKPublic& cKPublic) { return mlc::String("public", 6); },
[&](const CKPrivate& cKPrivate) { return mlc::String("private", 7); },
[&](const CKProtected& cKProtected) { return mlc::String("protected", 9); },
[&](const CKExplicit& cKExplicit) { return mlc::String("explicit", 8); },
[&](const CKMutable& cKMutable) { return mlc::String("mutable", 7); },
[&](const CKVolatile& cKVolatile) { return mlc::String("volatile", 8); },
[&](const CKFriend& cKFriend) { return mlc::String("friend", 6); },
[&](const CIdent& cIdent) { auto [name] = cIdent; return name; },
[&](const CLInt& cLInt) { auto [value] = cLInt; return mlc::to_string(value); },
[&](const CLStr& cLStr) { auto [value] = cLStr; return ((mlc::String("\"", 1) + value) + mlc::String("\"", 1)); },
[&](const CLChar& cLChar) { auto [value] = cLChar; return cpp_char_literal_lexeme(value); },
[&](const CLFloat& cLFloat) { auto [value] = cLFloat; return value; },
[&](const CLHex& cLHex) { auto [value] = cLHex; return value; },
[&](const CArrow& cArrow) { return mlc::String("->", 2); },
[&](const CScope& cScope) { return mlc::String("::", 2); },
[&](const CEq& cEq) { return mlc::String("==", 2); },
[&](const CNe& cNe) { return mlc::String("!=", 2); },
[&](const CLe& cLe) { return mlc::String("<=", 2); },
[&](const CGe& cGe) { return mlc::String(">=", 2); },
[&](const CAnd& cAnd) { return mlc::String("&&", 2); },
[&](const COr& cOr) { return mlc::String("||", 2); },
[&](const CInc& cInc) { return mlc::String("++", 2); },
[&](const CDec& cDec) { return mlc::String("--", 2); },
[&](const CLShift& cLShift) { return mlc::String("<<", 2); },
[&](const CRShift& cRShift) { return mlc::String(">>", 2); },
[&](const CLParen& cLParen) { return mlc::String("(", 1); },
[&](const CRParen& cRParen) { return mlc::String(")", 1); },
[&](const CLBrace& cLBrace) { return mlc::String("{", 1); },
[&](const CRBrace& cRBrace) { return mlc::String("}", 1); },
[&](const CLBracket& cLBracket) { return mlc::String("[", 1); },
[&](const CRBracket& cRBracket) { return mlc::String("]", 1); },
[&](const CLAngle& cLAngle) { return mlc::String("<", 1); },
[&](const CRAngle& cRAngle) { return mlc::String(">", 1); },
[&](const CHash& cHash) { return mlc::String("#", 1); },
[&](const COp& cOp) { auto [symbol] = cOp; return symbol; },
[&](const CEof& cEof) { return mlc::String("", 0); }
}, kind);
}
bool cpp_token_kind_is_eof(CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<CEof>(__match_subject)) {
const CEof& cEof = std::get<CEof>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
mlc::String cpp_token_to_source(CppToken token) noexcept{
  return ((token.leading_trivia + cpp_token_lexeme(token.kind)) + token.trailing_trivia);
}
mlc::String cpp_tokens_to_source(mlc::Array<CppToken> tokens) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto index = 0;
  while ((index < tokens.length()))   {
    auto token = tokens[index];
    if ((!cpp_token_kind_is_eof(token.kind)))     {
      parts.push_back(cpp_token_to_source(token));
    }
    (index = mlc::arith::checked_add(index, 1));
  }
  return parts.join(mlc::String("", 0));
}
bool cpp_token_kinds_equal(mlc::Array<CppToken> first, mlc::Array<CppToken> second) noexcept{
  if ((first.length() != second.length()))   {
    return false;
  } else   {
    auto index = 0;
    auto equal = true;
    while (((index < first.length()) && equal))     {
      if ((cpp_token_lexeme(first[index].kind) != cpp_token_lexeme(second[index].kind)))       {
        (equal = false);
      }
      (index = mlc::arith::checked_add(index, 1));
    }
    return equal;
  }
}

} // namespace cpp_tokens
