#ifndef CPP_TYPES_HPP
#define CPP_TYPES_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_ast.hpp"
#include "print.hpp"
#include "cpp_tokens.hpp"
namespace cpp_types {

struct CppTypedParseResult {
  std::shared_ptr<cpp_ast::CppType> type_node;
  int position;
};
struct CppTypeStringResult {
  mlc::String type_string;
  int position;
};
struct CppTypeQualifierScan {
  bool has_const;
  mlc::String name_prefix;
  int position;
};
struct CppQualifiedNameResult {
  mlc::String name;
  int position;
};
struct CppTemplateArgumentsResult {
  mlc::Array<std::shared_ptr<cpp_ast::CppType>> arguments;
  int position;
};
cpp_tokens::CppTokenKind cpp_type_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
bool cpp_type_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
bool cpp_type_is_const(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_volatile(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_unsigned(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_signed(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_long(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_short(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_scope(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_left_angle(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_star(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_ampersand(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_rvalue_reference(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_comma(cpp_tokens::CppTokenKind kind) noexcept;
mlc::String cpp_type_spelling_from_kind(cpp_tokens::CppTokenKind kind) noexcept;
bool cpp_type_is_specifier_start(cpp_tokens::CppTokenKind kind) noexcept;
mlc::String cpp_type_append_spelling(mlc::String existing, mlc::String piece) noexcept;
CppQualifiedNameResult cpp_type_join_qualified_name(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppTypeQualifierScan cpp_type_parse_qualifiers(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
mlc::String cpp_type_full_spelling(mlc::String name_prefix, mlc::String core_name) noexcept;
CppTemplateArgumentsResult cpp_type_parse_template_arguments(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppTypedParseResult cpp_type_parse_base(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String name_prefix) noexcept;
CppTypedParseResult cpp_type_parse_suffixes(mlc::Array<cpp_tokens::CppToken> tokens, int position, std::shared_ptr<cpp_ast::CppType> type_node) noexcept;
CppTypedParseResult parse_cpp_type(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppTypeStringResult parse_cpp_type_to_string(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

} // namespace cpp_types

#endif // CPP_TYPES_HPP
