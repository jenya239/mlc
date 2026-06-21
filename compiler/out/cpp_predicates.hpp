#ifndef CPP_PREDICATES_HPP
#define CPP_PREDICATES_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_tokens.hpp"

namespace cpp_predicates {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

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

} // namespace cpp_predicates

#endif // CPP_PREDICATES_HPP
