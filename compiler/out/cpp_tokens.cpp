#include "cpp_tokens.hpp"

namespace cpp_tokens {

bool is_cpp_reserved_keyword(mlc::String word) noexcept;

cpp_tokens::CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept;

cpp_tokens::CppTokenKind cpp_keyword_kind(mlc::String word) noexcept;

mlc::String cpp_token_lexeme(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_token_kind_is_eof(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_tokens_to_source(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

bool cpp_token_kinds_equal(mlc::Array<cpp_tokens::CppToken> first, mlc::Array<cpp_tokens::CppToken> second) noexcept;

bool is_cpp_reserved_keyword(mlc::String word) noexcept{return [&]() { if (word == mlc::String("auto")) { return true; } if (word == mlc::String("const")) { return true; } if (word == mlc::String("constexpr")) { return true; } if (word == mlc::String("if")) { return true; } if (word == mlc::String("else")) { return true; } if (word == mlc::String("while")) { return true; } if (word == mlc::String("for")) { return true; } if (word == mlc::String("return")) { return true; } if (word == mlc::String("break")) { return true; } if (word == mlc::String("struct")) { return true; } if (word == mlc::String("using")) { return true; } if (word == mlc::String("noexcept")) { return true; } if (word == mlc::String("inline")) { return true; } if (word == mlc::String("static")) { return true; } if (word == mlc::String("void")) { return true; } if (word == mlc::String("int")) { return true; } if (word == mlc::String("bool")) { return true; } if (word == mlc::String("char")) { return true; } if (word == mlc::String("true")) { return true; } if (word == mlc::String("false")) { return true; } if (word == mlc::String("nullptr")) { return true; } if (word == mlc::String("template")) { return true; } if (word == mlc::String("typename")) { return true; } if (word == mlc::String("namespace")) { return true; } if (word == mlc::String("new")) { return true; } if (word == mlc::String("delete")) { return true; } if (word == mlc::String("operator")) { return true; } if (word == mlc::String("class")) { return true; } if (word == mlc::String("enum")) { return true; } if (word == mlc::String("typedef")) { return true; } if (word == mlc::String("extern")) { return true; } if (word == mlc::String("signed")) { return true; } if (word == mlc::String("unsigned")) { return true; } if (word == mlc::String("long")) { return true; } if (word == mlc::String("short")) { return true; } if (word == mlc::String("float")) { return true; } if (word == mlc::String("double")) { return true; } if (word == mlc::String("virtual")) { return true; } if (word == mlc::String("override")) { return true; } if (word == mlc::String("final")) { return true; } if (word == mlc::String("public")) { return true; } if (word == mlc::String("private")) { return true; } if (word == mlc::String("protected")) { return true; } if (word == mlc::String("explicit")) { return true; } if (word == mlc::String("mutable")) { return true; } if (word == mlc::String("volatile")) { return true; } if (word == mlc::String("friend")) { return true; } return false; }();}

cpp_tokens::CppTokenKind reserved_cpp_keyword_kind(mlc::String word) noexcept{return [&]() -> cpp_tokens::CppTokenKind { if (word == mlc::String("auto")) { return (cpp_tokens::CKAuto{}); } if (word == mlc::String("const")) { return (cpp_tokens::CKConst{}); } if (word == mlc::String("constexpr")) { return (cpp_tokens::CKConstexpr{}); } if (word == mlc::String("if")) { return (cpp_tokens::CKIf{}); } if (word == mlc::String("else")) { return (cpp_tokens::CKElse{}); } if (word == mlc::String("while")) { return (cpp_tokens::CKWhile{}); } if (word == mlc::String("for")) { return (cpp_tokens::CKFor{}); } if (word == mlc::String("return")) { return (cpp_tokens::CKReturn{}); } if (word == mlc::String("break")) { return (cpp_tokens::CKBreak{}); } if (word == mlc::String("struct")) { return (cpp_tokens::CKStruct{}); } if (word == mlc::String("using")) { return (cpp_tokens::CKUsing{}); } if (word == mlc::String("noexcept")) { return (cpp_tokens::CKNoexcept{}); } if (word == mlc::String("inline")) { return (cpp_tokens::CKInline{}); } if (word == mlc::String("static")) { return (cpp_tokens::CKStatic{}); } if (word == mlc::String("void")) { return (cpp_tokens::CKVoid{}); } if (word == mlc::String("int")) { return (cpp_tokens::CKInt{}); } if (word == mlc::String("bool")) { return (cpp_tokens::CKBool{}); } if (word == mlc::String("char")) { return (cpp_tokens::CKChar{}); } if (word == mlc::String("true")) { return (cpp_tokens::CKTrue{}); } if (word == mlc::String("false")) { return (cpp_tokens::CKFalse{}); } if (word == mlc::String("nullptr")) { return (cpp_tokens::CKNullptr{}); } if (word == mlc::String("template")) { return (cpp_tokens::CKTemplate{}); } if (word == mlc::String("typename")) { return (cpp_tokens::CKTypename{}); } if (word == mlc::String("namespace")) { return (cpp_tokens::CKNamespace{}); } if (word == mlc::String("new")) { return (cpp_tokens::CKNew{}); } if (word == mlc::String("delete")) { return (cpp_tokens::CKDelete{}); } if (word == mlc::String("operator")) { return (cpp_tokens::CKOperator{}); } if (word == mlc::String("class")) { return (cpp_tokens::CKClass{}); } if (word == mlc::String("enum")) { return (cpp_tokens::CKEnum{}); } if (word == mlc::String("typedef")) { return (cpp_tokens::CKTypedef{}); } if (word == mlc::String("extern")) { return (cpp_tokens::CKExtern{}); } if (word == mlc::String("signed")) { return (cpp_tokens::CKSigned{}); } if (word == mlc::String("unsigned")) { return (cpp_tokens::CKUnsigned{}); } if (word == mlc::String("long")) { return (cpp_tokens::CKLong{}); } if (word == mlc::String("short")) { return (cpp_tokens::CKShort{}); } if (word == mlc::String("float")) { return (cpp_tokens::CKFloat{}); } if (word == mlc::String("double")) { return (cpp_tokens::CKDouble{}); } if (word == mlc::String("virtual")) { return (cpp_tokens::CKVirtual{}); } if (word == mlc::String("override")) { return (cpp_tokens::CKOverride{}); } if (word == mlc::String("final")) { return (cpp_tokens::CKFinal{}); } if (word == mlc::String("public")) { return (cpp_tokens::CKPublic{}); } if (word == mlc::String("private")) { return (cpp_tokens::CKPrivate{}); } if (word == mlc::String("protected")) { return (cpp_tokens::CKProtected{}); } if (word == mlc::String("explicit")) { return (cpp_tokens::CKExplicit{}); } if (word == mlc::String("mutable")) { return (cpp_tokens::CKMutable{}); } if (word == mlc::String("volatile")) { return (cpp_tokens::CKVolatile{}); } if (word == mlc::String("friend")) { return (cpp_tokens::CKFriend{}); } }();}

cpp_tokens::CppTokenKind cpp_keyword_kind(mlc::String word) noexcept{return is_cpp_reserved_keyword(word) ? cpp_tokens::CppTokenKind(reserved_cpp_keyword_kind(word)) : cpp_tokens::CppTokenKind(cpp_tokens::CIdent(word));}

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

mlc::String cpp_tokens_to_source(mlc::Array<cpp_tokens::CppToken> tokens) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < tokens.size()){
{
cpp_tokens::CppTokenKind kind = tokens[index].kind;
if (!cpp_token_kind_is_eof(kind)){
{
parts.push_back(cpp_token_lexeme(kind));
}
}
index = index + 1;
}
}
return parts.join(mlc::String(" "));
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
