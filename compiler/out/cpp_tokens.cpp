#include "cpp_tokens.hpp"

namespace cpp_tokens {

struct CppKeywordEntry {mlc::String name;cpp_tokens::CppTokenKind kind;};

cpp_tokens::CppToken cpp_token_make(cpp_tokens::CppTokenKind kind, int line, int column) noexcept;

cpp_tokens::CppToken cpp_token_with_trivia(cpp_tokens::CppToken token, mlc::String leading_trivia, mlc::String trailing_trivia) noexcept;

mlc::Array<cpp_tokens::CppKeywordEntry> cpp_keyword_entries() noexcept;

bool is_cpp_reserved_keyword(mlc::String word) noexcept;

cpp_tokens::CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept;

cpp_tokens::CppTokenKind cpp_keyword_kind(mlc::String word) noexcept;

mlc::String cpp_char_literal_lexeme(mlc::String character) noexcept;

mlc::String cpp_token_lexeme(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_token_kind_is_eof(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_token_to_source(cpp_tokens::CppToken token) noexcept;

mlc::String cpp_tokens_to_source(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_token_kinds_equal(mlc::Array<cpp_tokens::CppToken> first, mlc::Array<cpp_tokens::CppToken> second) noexcept;

cpp_tokens::CppToken cpp_token_make(cpp_tokens::CppTokenKind kind, int line, int column) noexcept{return cpp_tokens::CppToken{kind, line, column, mlc::String(""), mlc::String("")};}

cpp_tokens::CppToken cpp_token_with_trivia(cpp_tokens::CppToken token, mlc::String leading_trivia, mlc::String trailing_trivia) noexcept{return cpp_tokens::CppToken{token.kind, token.line, token.column, leading_trivia, trailing_trivia};}

mlc::Array<cpp_tokens::CppKeywordEntry> cpp_keyword_entries() noexcept{
mlc::Array<cpp_tokens::CppKeywordEntry> entries = mlc::Array<cpp_tokens::CppKeywordEntry>{cpp_tokens::CppKeywordEntry{mlc::String("auto"), (cpp_tokens::CKAuto{})}, cpp_tokens::CppKeywordEntry{mlc::String("const"), (cpp_tokens::CKConst{})}, cpp_tokens::CppKeywordEntry{mlc::String("constexpr"), (cpp_tokens::CKConstexpr{})}, cpp_tokens::CppKeywordEntry{mlc::String("if"), (cpp_tokens::CKIf{})}, cpp_tokens::CppKeywordEntry{mlc::String("else"), (cpp_tokens::CKElse{})}, cpp_tokens::CppKeywordEntry{mlc::String("while"), (cpp_tokens::CKWhile{})}, cpp_tokens::CppKeywordEntry{mlc::String("for"), (cpp_tokens::CKFor{})}, cpp_tokens::CppKeywordEntry{mlc::String("return"), (cpp_tokens::CKReturn{})}, cpp_tokens::CppKeywordEntry{mlc::String("break"), (cpp_tokens::CKBreak{})}, cpp_tokens::CppKeywordEntry{mlc::String("struct"), (cpp_tokens::CKStruct{})}, cpp_tokens::CppKeywordEntry{mlc::String("using"), (cpp_tokens::CKUsing{})}, cpp_tokens::CppKeywordEntry{mlc::String("noexcept"), (cpp_tokens::CKNoexcept{})}, cpp_tokens::CppKeywordEntry{mlc::String("inline"), (cpp_tokens::CKInline{})}, cpp_tokens::CppKeywordEntry{mlc::String("static"), (cpp_tokens::CKStatic{})}, cpp_tokens::CppKeywordEntry{mlc::String("void"), (cpp_tokens::CKVoid{})}, cpp_tokens::CppKeywordEntry{mlc::String("int"), (cpp_tokens::CKInt{})}, cpp_tokens::CppKeywordEntry{mlc::String("bool"), (cpp_tokens::CKBool{})}, cpp_tokens::CppKeywordEntry{mlc::String("char"), (cpp_tokens::CKChar{})}, cpp_tokens::CppKeywordEntry{mlc::String("true"), (cpp_tokens::CKTrue{})}, cpp_tokens::CppKeywordEntry{mlc::String("false"), (cpp_tokens::CKFalse{})}, cpp_tokens::CppKeywordEntry{mlc::String("nullptr"), (cpp_tokens::CKNullptr{})}, cpp_tokens::CppKeywordEntry{mlc::String("template"), (cpp_tokens::CKTemplate{})}, cpp_tokens::CppKeywordEntry{mlc::String("typename"), (cpp_tokens::CKTypename{})}, cpp_tokens::CppKeywordEntry{mlc::String("namespace"), (cpp_tokens::CKNamespace{})}, cpp_tokens::CppKeywordEntry{mlc::String("new"), (cpp_tokens::CKNew{})}, cpp_tokens::CppKeywordEntry{mlc::String("delete"), (cpp_tokens::CKDelete{})}, cpp_tokens::CppKeywordEntry{mlc::String("operator"), (cpp_tokens::CKOperator{})}, cpp_tokens::CppKeywordEntry{mlc::String("class"), (cpp_tokens::CKClass{})}, cpp_tokens::CppKeywordEntry{mlc::String("enum"), (cpp_tokens::CKEnum{})}, cpp_tokens::CppKeywordEntry{mlc::String("typedef"), (cpp_tokens::CKTypedef{})}, cpp_tokens::CppKeywordEntry{mlc::String("extern"), (cpp_tokens::CKExtern{})}, cpp_tokens::CppKeywordEntry{mlc::String("signed"), (cpp_tokens::CKSigned{})}, cpp_tokens::CppKeywordEntry{mlc::String("unsigned"), (cpp_tokens::CKUnsigned{})}, cpp_tokens::CppKeywordEntry{mlc::String("long"), (cpp_tokens::CKLong{})}, cpp_tokens::CppKeywordEntry{mlc::String("short"), (cpp_tokens::CKShort{})}, cpp_tokens::CppKeywordEntry{mlc::String("float"), (cpp_tokens::CKFloat{})}, cpp_tokens::CppKeywordEntry{mlc::String("double"), (cpp_tokens::CKDouble{})}, cpp_tokens::CppKeywordEntry{mlc::String("virtual"), (cpp_tokens::CKVirtual{})}, cpp_tokens::CppKeywordEntry{mlc::String("override"), (cpp_tokens::CKOverride{})}, cpp_tokens::CppKeywordEntry{mlc::String("final"), (cpp_tokens::CKFinal{})}, cpp_tokens::CppKeywordEntry{mlc::String("public"), (cpp_tokens::CKPublic{})}, cpp_tokens::CppKeywordEntry{mlc::String("private"), (cpp_tokens::CKPrivate{})}, cpp_tokens::CppKeywordEntry{mlc::String("protected"), (cpp_tokens::CKProtected{})}, cpp_tokens::CppKeywordEntry{mlc::String("explicit"), (cpp_tokens::CKExplicit{})}, cpp_tokens::CppKeywordEntry{mlc::String("mutable"), (cpp_tokens::CKMutable{})}, cpp_tokens::CppKeywordEntry{mlc::String("volatile"), (cpp_tokens::CKVolatile{})}, cpp_tokens::CppKeywordEntry{mlc::String("friend"), (cpp_tokens::CKFriend{})}};
return entries;
}

bool is_cpp_reserved_keyword(mlc::String word) noexcept{return cpp_keyword_entries().any([word](cpp_tokens::CppKeywordEntry entry) mutable { return entry.name == word; });}

cpp_tokens::CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept{
mlc::Array<cpp_tokens::CppKeywordEntry> entries = cpp_keyword_entries();
int index = 0;
while (index < entries.size()){
{
if (entries[index].name == word){
{
return entries[index].kind;
}
}
index = index + 1;
}
}
return (cpp_tokens::CKAuto{});
}

cpp_tokens::CppTokenKind cpp_keyword_kind(mlc::String word) noexcept{return is_cpp_reserved_keyword(word) ? cpp_tokens::CppTokenKind(reserved_cpp_keyword_kind(word)) : cpp_tokens::CppTokenKind(cpp_tokens::CIdent(word));}

mlc::String cpp_char_literal_lexeme(mlc::String character) noexcept{
mlc::String escaped = character == mlc::String("\\") ? mlc::String("\\\\") : character == mlc::String("'") ? mlc::String("\\'") : character == mlc::String("\n") ? mlc::String("\\n") : character == mlc::String("\r") ? mlc::String("\\r") : character == mlc::String("\t") ? mlc::String("\\t") : character == mlc::String("\0", 1) ? mlc::String("\\0") : character;
return mlc::String("'") + escaped + mlc::String("'");
}

mlc::String cpp_token_lexeme(cpp_tokens::CppTokenKind kind) noexcept{return std::visit(overloaded{
  [&](const CKAuto& ckauto) -> mlc::String { return mlc::String("auto"); },
  [&](const CKConst& ckconst) -> mlc::String { return mlc::String("const"); },
  [&](const CKConstexpr& ckconstexpr) -> mlc::String { return mlc::String("constexpr"); },
  [&](const CKIf& ckif) -> mlc::String { return mlc::String("if"); },
  [&](const CKElse& ckelse) -> mlc::String { return mlc::String("else"); },
  [&](const CKWhile& ckwhile) -> mlc::String { return mlc::String("while"); },
  [&](const CKFor& ckfor) -> mlc::String { return mlc::String("for"); },
  [&](const CKReturn& ckreturn) -> mlc::String { return mlc::String("return"); },
  [&](const CKBreak& ckbreak) -> mlc::String { return mlc::String("break"); },
  [&](const CKStruct& ckstruct) -> mlc::String { return mlc::String("struct"); },
  [&](const CKUsing& ckusing) -> mlc::String { return mlc::String("using"); },
  [&](const CKNoexcept& cknoexcept) -> mlc::String { return mlc::String("noexcept"); },
  [&](const CKInline& ckinline) -> mlc::String { return mlc::String("inline"); },
  [&](const CKStatic& ckstatic) -> mlc::String { return mlc::String("static"); },
  [&](const CKVoid& ckvoid) -> mlc::String { return mlc::String("void"); },
  [&](const CKInt& ckint) -> mlc::String { return mlc::String("int"); },
  [&](const CKBool& ckbool) -> mlc::String { return mlc::String("bool"); },
  [&](const CKChar& ckchar) -> mlc::String { return mlc::String("char"); },
  [&](const CKTrue& cktrue) -> mlc::String { return mlc::String("true"); },
  [&](const CKFalse& ckfalse) -> mlc::String { return mlc::String("false"); },
  [&](const CKNullptr& cknullptr) -> mlc::String { return mlc::String("nullptr"); },
  [&](const CKTemplate& cktemplate) -> mlc::String { return mlc::String("template"); },
  [&](const CKTypename& cktypename) -> mlc::String { return mlc::String("typename"); },
  [&](const CKNamespace& cknamespace) -> mlc::String { return mlc::String("namespace"); },
  [&](const CKNew& cknew) -> mlc::String { return mlc::String("new"); },
  [&](const CKDelete& ckdelete) -> mlc::String { return mlc::String("delete"); },
  [&](const CKOperator& ckoperator) -> mlc::String { return mlc::String("operator"); },
  [&](const CKClass& ckclass) -> mlc::String { return mlc::String("class"); },
  [&](const CKEnum& ckenum) -> mlc::String { return mlc::String("enum"); },
  [&](const CKTypedef& cktypedef) -> mlc::String { return mlc::String("typedef"); },
  [&](const CKExtern& ckextern) -> mlc::String { return mlc::String("extern"); },
  [&](const CKSigned& cksigned) -> mlc::String { return mlc::String("signed"); },
  [&](const CKUnsigned& ckunsigned) -> mlc::String { return mlc::String("unsigned"); },
  [&](const CKLong& cklong) -> mlc::String { return mlc::String("long"); },
  [&](const CKShort& ckshort) -> mlc::String { return mlc::String("short"); },
  [&](const CKFloat& ckfloat) -> mlc::String { return mlc::String("float"); },
  [&](const CKDouble& ckdouble) -> mlc::String { return mlc::String("double"); },
  [&](const CKVirtual& ckvirtual) -> mlc::String { return mlc::String("virtual"); },
  [&](const CKOverride& ckoverride) -> mlc::String { return mlc::String("override"); },
  [&](const CKFinal& ckfinal) -> mlc::String { return mlc::String("final"); },
  [&](const CKPublic& ckpublic) -> mlc::String { return mlc::String("public"); },
  [&](const CKPrivate& ckprivate) -> mlc::String { return mlc::String("private"); },
  [&](const CKProtected& ckprotected) -> mlc::String { return mlc::String("protected"); },
  [&](const CKExplicit& ckexplicit) -> mlc::String { return mlc::String("explicit"); },
  [&](const CKMutable& ckmutable) -> mlc::String { return mlc::String("mutable"); },
  [&](const CKVolatile& ckvolatile) -> mlc::String { return mlc::String("volatile"); },
  [&](const CKFriend& ckfriend) -> mlc::String { return mlc::String("friend"); },
  [&](const CIdent& cident) -> mlc::String { auto [name] = cident; return name; },
  [&](const CLInt& clint) -> mlc::String { auto [value] = clint; return mlc::to_string(value); },
  [&](const CLStr& clstr) -> mlc::String { auto [value] = clstr; return mlc::String("\"") + value + mlc::String("\""); },
  [&](const CLChar& clchar) -> mlc::String { auto [value] = clchar; return cpp_char_literal_lexeme(value); },
  [&](const CLFloat& clfloat) -> mlc::String { auto [value] = clfloat; return value; },
  [&](const CLHex& clhex) -> mlc::String { auto [value] = clhex; return value; },
  [&](const CArrow& carrow) -> mlc::String { return mlc::String("->"); },
  [&](const CScope& cscope) -> mlc::String { return mlc::String("::"); },
  [&](const CEq& ceq) -> mlc::String { return mlc::String("=="); },
  [&](const CNe& cne) -> mlc::String { return mlc::String("!="); },
  [&](const CLe& cle) -> mlc::String { return mlc::String("<="); },
  [&](const CGe& cge) -> mlc::String { return mlc::String(">="); },
  [&](const CAnd& cand) -> mlc::String { return mlc::String("&&"); },
  [&](const COr& cor) -> mlc::String { return mlc::String("||"); },
  [&](const CInc& cinc) -> mlc::String { return mlc::String("++"); },
  [&](const CDec& cdec) -> mlc::String { return mlc::String("--"); },
  [&](const CLShift& clshift) -> mlc::String { return mlc::String("<<"); },
  [&](const CRShift& crshift) -> mlc::String { return mlc::String(">>"); },
  [&](const CLParen& clparen) -> mlc::String { return mlc::String("("); },
  [&](const CRParen& crparen) -> mlc::String { return mlc::String(")"); },
  [&](const CLBrace& clbrace) -> mlc::String { return mlc::String("{"); },
  [&](const CRBrace& crbrace) -> mlc::String { return mlc::String("}"); },
  [&](const CLBracket& clbracket) -> mlc::String { return mlc::String("["); },
  [&](const CRBracket& crbracket) -> mlc::String { return mlc::String("]"); },
  [&](const CLAngle& clangle) -> mlc::String { return mlc::String("<"); },
  [&](const CRAngle& crangle) -> mlc::String { return mlc::String(">"); },
  [&](const CHash& chash) -> mlc::String { return mlc::String("#"); },
  [&](const COp& cop) -> mlc::String { auto [symbol] = cop; return symbol; },
  [&](const CEof& ceof) -> mlc::String { return mlc::String(""); }
}, kind);}

bool cpp_token_kind_is_eof(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CEof>(kind)) {  return true; } return false; }();}

mlc::String cpp_token_to_source(cpp_tokens::CppToken token) noexcept{return token.leading_trivia + cpp_token_lexeme(token.kind) + token.trailing_trivia;}

mlc::String cpp_tokens_to_source(mlc::Array<cpp_tokens::CppToken> tokens) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < tokens.size()){
{
cpp_tokens::CppToken token = tokens[index];
if (!cpp_token_kind_is_eof(token.kind)){
{
parts.push_back(cpp_token_to_source(token));
}
}
index = index + 1;
}
}
return parts.join(mlc::String(""));
}

bool cpp_token_kinds_equal(mlc::Array<cpp_tokens::CppToken> first, mlc::Array<cpp_tokens::CppToken> second) noexcept{
return first.size() != second.size() ? false : [&]() -> bool { 
  int index = 0;
  bool equal = true;
  while (index < first.size() && equal){
{
if (cpp_token_lexeme(first[index].kind) != cpp_token_lexeme(second[index].kind)){
{
equal = false;
}
}
index = index + 1;
}
}
  return equal;
 }();
}

} // namespace cpp_tokens
