#ifndef CPP_PREDICATES_HPP
#define CPP_PREDICATES_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_tokens.hpp"
namespace cpp_predicates {

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

} // namespace cpp_predicates

#endif // CPP_PREDICATES_HPP
