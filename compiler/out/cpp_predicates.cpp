#include "cpp_predicates.hpp"

#include "cpp_tokens.hpp"

namespace cpp_predicates {

using namespace cpp_tokens;

cpp_tokens::CppTokenKind cpp_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

bool cpp_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

mlc::String cpp_get_identifier(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_string(cpp_tokens::CppTokenKind kind) noexcept;

mlc::String cpp_get_operator(cpp_tokens::CppTokenKind kind) noexcept;

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

bool cpp_is_comma(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(","); } return false; }();}

bool cpp_is_void_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKVoid>(kind)) {  return true; } return false; }();}

bool cpp_is_int_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKInt>(kind)) {  return true; } return false; }();}

bool cpp_is_bool_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKBool>(kind)) {  return true; } return false; }();}

bool cpp_is_char_keyword(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKChar>(kind)) {  return true; } return false; }();}

mlc::String cpp_type_part_string(cpp_tokens::CppTokenKind kind) noexcept{
return cpp_get_identifier(kind) != mlc::String("") ? cpp_get_identifier(kind) : cpp_get_string(kind) != mlc::String("") ? mlc::String("\"") + cpp_get_string(kind) + mlc::String("\"") : cpp_is_angle(kind) ? mlc::String("<") : cpp_is_right_angle(kind) ? mlc::String(">") : cpp_is_scope(kind) ? mlc::String("::") : cpp_is_void_keyword(kind) ? mlc::String("void") : cpp_is_int_keyword(kind) ? mlc::String("int") : cpp_is_bool_keyword(kind) ? mlc::String("bool") : cpp_is_char_keyword(kind) ? mlc::String("char") : cpp_get_operator(kind) != mlc::String("") ? cpp_get_operator(kind) : mlc::String("");
}

} // namespace cpp_predicates
