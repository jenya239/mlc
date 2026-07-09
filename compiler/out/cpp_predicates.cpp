#include "cpp_predicates.hpp"

#include "cpp_tokens.hpp"

namespace cpp_predicates {

using namespace cpp_tokens;

cpp_tokens::CppTokenKind cpp_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

bool cpp_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

mlc::String cpp_get_identifier(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_string(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_operator(cpp_tokens::CppTokenKind kind) noexcept;

int cpp_get_integer(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_char_literal(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_float_literal(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_hex_literal(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_integer_literal(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_char_literal(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_float_literal(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_hex_literal(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_true_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_false_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_nullptr_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_arrow(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_or_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_and_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_equals_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_not_equals_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_less_equals_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_greater_equals_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_left_shift_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_right_shift_operator(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_question(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_colon(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_dot(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_left_bracket(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_right_bracket(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_hash(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_identifier(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_string(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_angle(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_scope(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_semicolon(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_equals(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_left_paren(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_right_paren(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_left_brace(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_right_brace(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_stop_for_type(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_using_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_struct_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_class_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_enum_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_namespace_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_template_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_typedef_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_extern_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_public_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_protected_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_private_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_virtual_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_static_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_override_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_explicit_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_const_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_typename_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_comma(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_void_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_int_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_bool_keyword(cpp_tokens::CppTokenKind kind) noexcept;

bool cpp_is_char_keyword(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_type_part_string(cpp_tokens::CppTokenKind kind) noexcept;

cpp_tokens::CppTokenKind cpp_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return position < tokens.size() ? cpp_tokens::CppTokenKind(tokens[position].kind) : cpp_tokens::CppTokenKind((cpp_tokens::CEof{}));}

bool cpp_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CEof>(cpp_token_kind_at(tokens, position))) {  return true; } return false; }();}

mlc::String cpp_get_identifier(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CIdent>(kind)) { auto _v_cident = std::get<cpp_tokens::CIdent>(kind); auto [name] = _v_cident; return name; } return mlc::String(""); }();}

mlc::String cpp_get_string(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CLStr>(kind)) { auto _v_clstr = std::get<cpp_tokens::CLStr>(kind); auto [value] = _v_clstr; return value; } return mlc::String(""); }();}

mlc::String cpp_get_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol; } return mlc::String(""); }();}

int cpp_get_integer(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLInt>(kind)) { auto _v_clint = std::get<cpp_tokens::CLInt>(kind); auto [value] = _v_clint; return value; } return 0; }();}

mlc::String cpp_get_char_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CLChar>(kind)) { auto _v_clchar = std::get<cpp_tokens::CLChar>(kind); auto [value] = _v_clchar; return value; } return mlc::String(""); }();}

mlc::String cpp_get_float_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CLFloat>(kind)) { auto _v_clfloat = std::get<cpp_tokens::CLFloat>(kind); auto [value] = _v_clfloat; return value; } return mlc::String(""); }();}

mlc::String cpp_get_hex_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CLHex>(kind)) { auto _v_clhex = std::get<cpp_tokens::CLHex>(kind); auto [value] = _v_clhex; return value; } return mlc::String(""); }();}

bool cpp_is_integer_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLInt>(kind)) { auto _v_clint = std::get<cpp_tokens::CLInt>(kind); auto [_w0] = _v_clint; return true; } return false; }();}

bool cpp_is_char_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLChar>(kind)) { auto _v_clchar = std::get<cpp_tokens::CLChar>(kind); auto [_w0] = _v_clchar; return true; } return false; }();}

bool cpp_is_float_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLFloat>(kind)) { auto _v_clfloat = std::get<cpp_tokens::CLFloat>(kind); auto [_w0] = _v_clfloat; return true; } return false; }();}

bool cpp_is_hex_literal(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLHex>(kind)) { auto _v_clhex = std::get<cpp_tokens::CLHex>(kind); auto [_w0] = _v_clhex; return true; } return false; }();}

bool cpp_is_true_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKTrue>(kind)) {  return true; } return false; }();}

bool cpp_is_false_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKFalse>(kind)) {  return true; } return false; }();}

bool cpp_is_nullptr_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKNullptr>(kind)) {  return true; } return false; }();}

bool cpp_is_arrow(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CArrow>(kind)) {  return true; } return false; }();}

bool cpp_is_or_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COr>(kind)) {  return true; } return false; }();}

bool cpp_is_and_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CAnd>(kind)) {  return true; } return false; }();}

bool cpp_is_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CEq>(kind)) {  return true; } return false; }();}

bool cpp_is_not_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CNe>(kind)) {  return true; } return false; }();}

bool cpp_is_less_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLe>(kind)) {  return true; } return false; }();}

bool cpp_is_greater_equals_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CGe>(kind)) {  return true; } return false; }();}

bool cpp_is_left_shift_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLShift>(kind)) {  return true; } return false; }();}

bool cpp_is_right_shift_operator(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRShift>(kind)) {  return true; } return false; }();}

bool cpp_is_question(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String("?"); } return false; }();}

bool cpp_is_colon(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(":"); } return false; }();}

bool cpp_is_dot(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String("."); } return false; }();}

bool cpp_is_left_bracket(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLBracket>(kind)) {  return true; } return false; }();}

bool cpp_is_right_bracket(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRBracket>(kind)) {  return true; } return false; }();}

bool cpp_is_hash(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CHash>(kind)) {  return true; } return false; }();}

bool cpp_is_identifier(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CIdent>(kind)) { auto _v_cident = std::get<cpp_tokens::CIdent>(kind); auto [_w0] = _v_cident; return true; } return false; }();}

bool cpp_is_string(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLStr>(kind)) { auto _v_clstr = std::get<cpp_tokens::CLStr>(kind); auto [_w0] = _v_clstr; return true; } return false; }();}

bool cpp_is_angle(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLAngle>(kind)) {  return true; } return false; }();}

bool cpp_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRAngle>(kind)) {  return true; } return false; }();}

bool cpp_is_scope(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CScope>(kind)) {  return true; } return false; }();}

bool cpp_is_semicolon(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(";"); } return false; }();}

bool cpp_is_equals(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String("="); } return false; }();}

bool cpp_is_left_paren(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLParen>(kind)) {  return true; } return false; }();}

bool cpp_is_right_paren(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRParen>(kind)) {  return true; } return false; }();}

bool cpp_is_left_brace(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLBrace>(kind)) {  return true; } return false; }();}

bool cpp_is_right_brace(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRBrace>(kind)) {  return true; } return false; }();}

bool cpp_is_stop_for_type(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(";") || symbol == mlc::String(",") || symbol == mlc::String(")"); } return false; }();}

bool cpp_is_using_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKUsing>(kind)) {  return true; } return false; }();}

bool cpp_is_struct_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKStruct>(kind)) {  return true; } return false; }();}

bool cpp_is_class_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKClass>(kind)) {  return true; } return false; }();}

bool cpp_is_enum_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKEnum>(kind)) {  return true; } return false; }();}

bool cpp_is_namespace_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKNamespace>(kind)) {  return true; } return false; }();}

bool cpp_is_template_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKTemplate>(kind)) {  return true; } return false; }();}

bool cpp_is_typedef_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKTypedef>(kind)) {  return true; } return false; }();}

bool cpp_is_extern_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKExtern>(kind)) {  return true; } return false; }();}

bool cpp_is_public_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKPublic>(kind)) {  return true; } return false; }();}

bool cpp_is_protected_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKProtected>(kind)) {  return true; } return false; }();}

bool cpp_is_private_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKPrivate>(kind)) {  return true; } return false; }();}

bool cpp_is_virtual_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKVirtual>(kind)) {  return true; } return false; }();}

bool cpp_is_static_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKStatic>(kind)) {  return true; } return false; }();}

bool cpp_is_override_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKOverride>(kind)) {  return true; } return false; }();}

bool cpp_is_explicit_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKExplicit>(kind)) {  return true; } return false; }();}

bool cpp_is_const_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKConst>(kind)) {  return true; } return false; }();}

bool cpp_is_typename_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKTypename>(kind)) {  return true; } return false; }();}

bool cpp_is_comma(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(","); } return false; }();}

bool cpp_is_void_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKVoid>(kind)) {  return true; } return false; }();}

bool cpp_is_int_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKInt>(kind)) {  return true; } return false; }();}

bool cpp_is_bool_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKBool>(kind)) {  return true; } return false; }();}

bool cpp_is_char_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKChar>(kind)) {  return true; } return false; }();}

mlc::String cpp_type_part_string(cpp_tokens::CppTokenKind kind) noexcept{
return cpp_get_identifier(kind) != mlc::String("") ? cpp_get_identifier(kind) : cpp_get_string(kind) != mlc::String("") ? mlc::String("\"") + cpp_get_string(kind) + mlc::String("\"") : cpp_is_angle(kind) ? mlc::String("<") : cpp_is_right_angle(kind) ? mlc::String(">") : cpp_is_scope(kind) ? mlc::String("::") : cpp_is_void_keyword(kind) ? mlc::String("void") : cpp_is_int_keyword(kind) ? mlc::String("int") : cpp_is_bool_keyword(kind) ? mlc::String("bool") : cpp_is_char_keyword(kind) ? mlc::String("char") : cpp_is_typename_keyword(kind) ? mlc::String("typename") : cpp_get_operator(kind) != mlc::String("") ? cpp_get_operator(kind) : mlc::String("");
}

} // namespace cpp_predicates
