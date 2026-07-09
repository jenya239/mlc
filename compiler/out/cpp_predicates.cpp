#define main mlc_user_main
#include "cpp_predicates.hpp"

#include "cpp_tokens.hpp"

namespace cpp_predicates {

using namespace cpp_tokens;

cpp_tokens::CppTokenKind cpp_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  if ((position < tokens.length()))   {
    return tokens[position].kind;
  } else   {
    return cpp_tokens::CEof{};
  }
}
bool cpp_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  return [&]() -> bool {
auto __match_subject = cpp_token_kind_at(tokens, position);
if (std::holds_alternative<cpp_tokens::CEof>(__match_subject)) {
const cpp_tokens::CEof& cEof = std::get<cpp_tokens::CEof>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
mlc::String cpp_get_identifier(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CIdent>(__match_subject)) {
const cpp_tokens::CIdent& cIdent = std::get<cpp_tokens::CIdent>(__match_subject);
auto [name] = cIdent; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String cpp_get_string(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLStr>(__match_subject)) {
const cpp_tokens::CLStr& cLStr = std::get<cpp_tokens::CLStr>(__match_subject);
auto [value] = cLStr; return value;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String cpp_get_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return symbol;
}
return mlc::String("", 0);
std::abort();
}();
}
int cpp_get_integer(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> int {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLInt>(__match_subject)) {
const cpp_tokens::CLInt& cLInt = std::get<cpp_tokens::CLInt>(__match_subject);
auto [value] = cLInt; return value;
}
return 0;
std::abort();
}();
}
mlc::String cpp_get_char_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLChar>(__match_subject)) {
const cpp_tokens::CLChar& cLChar = std::get<cpp_tokens::CLChar>(__match_subject);
auto [value] = cLChar; return value;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String cpp_get_float_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLFloat>(__match_subject)) {
const cpp_tokens::CLFloat& cLFloat = std::get<cpp_tokens::CLFloat>(__match_subject);
auto [value] = cLFloat; return value;
}
return mlc::String("", 0);
std::abort();
}();
}
mlc::String cpp_get_hex_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLHex>(__match_subject)) {
const cpp_tokens::CLHex& cLHex = std::get<cpp_tokens::CLHex>(__match_subject);
auto [value] = cLHex; return value;
}
return mlc::String("", 0);
std::abort();
}();
}
bool cpp_is_integer_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLInt>(__match_subject)) {
const cpp_tokens::CLInt& cLInt = std::get<cpp_tokens::CLInt>(__match_subject);
auto [__0] = cLInt; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_char_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLChar>(__match_subject)) {
const cpp_tokens::CLChar& cLChar = std::get<cpp_tokens::CLChar>(__match_subject);
auto [__0] = cLChar; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_float_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLFloat>(__match_subject)) {
const cpp_tokens::CLFloat& cLFloat = std::get<cpp_tokens::CLFloat>(__match_subject);
auto [__0] = cLFloat; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_hex_literal(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLHex>(__match_subject)) {
const cpp_tokens::CLHex& cLHex = std::get<cpp_tokens::CLHex>(__match_subject);
auto [__0] = cLHex; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_true_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKTrue>(__match_subject)) {
const cpp_tokens::CKTrue& cKTrue = std::get<cpp_tokens::CKTrue>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_false_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKFalse>(__match_subject)) {
const cpp_tokens::CKFalse& cKFalse = std::get<cpp_tokens::CKFalse>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_nullptr_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKNullptr>(__match_subject)) {
const cpp_tokens::CKNullptr& cKNullptr = std::get<cpp_tokens::CKNullptr>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_arrow(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CArrow>(__match_subject)) {
const cpp_tokens::CArrow& cArrow = std::get<cpp_tokens::CArrow>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_or_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COr>(__match_subject)) {
const cpp_tokens::COr& cOr = std::get<cpp_tokens::COr>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_and_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CAnd>(__match_subject)) {
const cpp_tokens::CAnd& cAnd = std::get<cpp_tokens::CAnd>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CEq>(__match_subject)) {
const cpp_tokens::CEq& cEq = std::get<cpp_tokens::CEq>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_not_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CNe>(__match_subject)) {
const cpp_tokens::CNe& cNe = std::get<cpp_tokens::CNe>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_less_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLe>(__match_subject)) {
const cpp_tokens::CLe& cLe = std::get<cpp_tokens::CLe>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_greater_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CGe>(__match_subject)) {
const cpp_tokens::CGe& cGe = std::get<cpp_tokens::CGe>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_left_shift_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLShift>(__match_subject)) {
const cpp_tokens::CLShift& cLShift = std::get<cpp_tokens::CLShift>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_right_shift_operator(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CRShift>(__match_subject)) {
const cpp_tokens::CRShift& cRShift = std::get<cpp_tokens::CRShift>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_question(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String("?", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_colon(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String(":", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_dot(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String(".", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_left_bracket(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLBracket>(__match_subject)) {
const cpp_tokens::CLBracket& cLBracket = std::get<cpp_tokens::CLBracket>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_right_bracket(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CRBracket>(__match_subject)) {
const cpp_tokens::CRBracket& cRBracket = std::get<cpp_tokens::CRBracket>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_hash(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CHash>(__match_subject)) {
const cpp_tokens::CHash& cHash = std::get<cpp_tokens::CHash>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_identifier(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CIdent>(__match_subject)) {
const cpp_tokens::CIdent& cIdent = std::get<cpp_tokens::CIdent>(__match_subject);
auto [__0] = cIdent; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_string(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLStr>(__match_subject)) {
const cpp_tokens::CLStr& cLStr = std::get<cpp_tokens::CLStr>(__match_subject);
auto [__0] = cLStr; return true;
}
return false;
std::abort();
}();
}
bool cpp_is_angle(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLAngle>(__match_subject)) {
const cpp_tokens::CLAngle& cLAngle = std::get<cpp_tokens::CLAngle>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CRAngle>(__match_subject)) {
const cpp_tokens::CRAngle& cRAngle = std::get<cpp_tokens::CRAngle>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_scope(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CScope>(__match_subject)) {
const cpp_tokens::CScope& cScope = std::get<cpp_tokens::CScope>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_semicolon(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String(";", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_equals(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String("=", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_left_paren(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLParen>(__match_subject)) {
const cpp_tokens::CLParen& cLParen = std::get<cpp_tokens::CLParen>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_right_paren(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CRParen>(__match_subject)) {
const cpp_tokens::CRParen& cRParen = std::get<cpp_tokens::CRParen>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_left_brace(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CLBrace>(__match_subject)) {
const cpp_tokens::CLBrace& cLBrace = std::get<cpp_tokens::CLBrace>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_right_brace(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CRBrace>(__match_subject)) {
const cpp_tokens::CRBrace& cRBrace = std::get<cpp_tokens::CRBrace>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_stop_for_type(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (((symbol == mlc::String(";", 1)) || (symbol == mlc::String(",", 1))) || (symbol == mlc::String(")", 1)));
}
return false;
std::abort();
}();
}
bool cpp_is_using_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKUsing>(__match_subject)) {
const cpp_tokens::CKUsing& cKUsing = std::get<cpp_tokens::CKUsing>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_struct_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKStruct>(__match_subject)) {
const cpp_tokens::CKStruct& cKStruct = std::get<cpp_tokens::CKStruct>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_class_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKClass>(__match_subject)) {
const cpp_tokens::CKClass& cKClass = std::get<cpp_tokens::CKClass>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_enum_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKEnum>(__match_subject)) {
const cpp_tokens::CKEnum& cKEnum = std::get<cpp_tokens::CKEnum>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_namespace_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKNamespace>(__match_subject)) {
const cpp_tokens::CKNamespace& cKNamespace = std::get<cpp_tokens::CKNamespace>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_template_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKTemplate>(__match_subject)) {
const cpp_tokens::CKTemplate& cKTemplate = std::get<cpp_tokens::CKTemplate>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_typedef_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKTypedef>(__match_subject)) {
const cpp_tokens::CKTypedef& cKTypedef = std::get<cpp_tokens::CKTypedef>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_extern_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKExtern>(__match_subject)) {
const cpp_tokens::CKExtern& cKExtern = std::get<cpp_tokens::CKExtern>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_public_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKPublic>(__match_subject)) {
const cpp_tokens::CKPublic& cKPublic = std::get<cpp_tokens::CKPublic>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_protected_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKProtected>(__match_subject)) {
const cpp_tokens::CKProtected& cKProtected = std::get<cpp_tokens::CKProtected>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_private_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKPrivate>(__match_subject)) {
const cpp_tokens::CKPrivate& cKPrivate = std::get<cpp_tokens::CKPrivate>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_virtual_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKVirtual>(__match_subject)) {
const cpp_tokens::CKVirtual& cKVirtual = std::get<cpp_tokens::CKVirtual>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_static_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKStatic>(__match_subject)) {
const cpp_tokens::CKStatic& cKStatic = std::get<cpp_tokens::CKStatic>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_override_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKOverride>(__match_subject)) {
const cpp_tokens::CKOverride& cKOverride = std::get<cpp_tokens::CKOverride>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_explicit_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKExplicit>(__match_subject)) {
const cpp_tokens::CKExplicit& cKExplicit = std::get<cpp_tokens::CKExplicit>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_const_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKConst>(__match_subject)) {
const cpp_tokens::CKConst& cKConst = std::get<cpp_tokens::CKConst>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_typename_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKTypename>(__match_subject)) {
const cpp_tokens::CKTypename& cKTypename = std::get<cpp_tokens::CKTypename>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_comma(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String(",", 1));
}
return false;
std::abort();
}();
}
bool cpp_is_void_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKVoid>(__match_subject)) {
const cpp_tokens::CKVoid& cKVoid = std::get<cpp_tokens::CKVoid>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_int_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKInt>(__match_subject)) {
const cpp_tokens::CKInt& cKInt = std::get<cpp_tokens::CKInt>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_bool_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKBool>(__match_subject)) {
const cpp_tokens::CKBool& cKBool = std::get<cpp_tokens::CKBool>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_is_char_keyword(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKChar>(__match_subject)) {
const cpp_tokens::CKChar& cKChar = std::get<cpp_tokens::CKChar>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
mlc::String cpp_type_part_string(cpp_tokens::CppTokenKind kind) noexcept{
  if ((cpp_get_identifier(kind) != mlc::String("", 0)))   {
    return cpp_get_identifier(kind);
  } else if ((cpp_get_string(kind) != mlc::String("", 0)))   {
    return ((mlc::String("\"", 1) + cpp_get_string(kind)) + mlc::String("\"", 1));
  } else if (cpp_is_angle(kind))   {
    return mlc::String("<", 1);
  } else if (cpp_is_right_angle(kind))   {
    return mlc::String(">", 1);
  } else if (cpp_is_scope(kind))   {
    return mlc::String("::", 2);
  } else if (cpp_is_void_keyword(kind))   {
    return mlc::String("void", 4);
  } else if (cpp_is_int_keyword(kind))   {
    return mlc::String("int", 3);
  } else if (cpp_is_bool_keyword(kind))   {
    return mlc::String("bool", 4);
  } else if (cpp_is_char_keyword(kind))   {
    return mlc::String("char", 4);
  } else if (cpp_is_typename_keyword(kind))   {
    return mlc::String("typename", 8);
  } else if ((cpp_get_operator(kind) != mlc::String("", 0)))   {
    return cpp_get_operator(kind);
  } else   {
    return mlc::String("", 0);
  }
}

} // namespace cpp_predicates
