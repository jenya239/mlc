#define main mlc_user_main
#include "cpp_types.hpp"

#include "cpp_ast.hpp"
#include "print.hpp"
#include "cpp_tokens.hpp"

namespace cpp_types {

using namespace cpp_ast;
using namespace print;
using namespace cpp_tokens;

cpp_tokens::CppTokenKind cpp_type_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  if ((position < tokens.length()))   {
    return tokens[position].kind;
  } else   {
    return cpp_tokens::CEof{};
  }
}
bool cpp_type_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  return [&]() -> bool {
auto __match_subject = cpp_type_token_kind_at(tokens, position);
if (std::holds_alternative<cpp_tokens::CEof>(__match_subject)) {
const cpp_tokens::CEof& cEof = std::get<cpp_tokens::CEof>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_const(cpp_tokens::CppTokenKind kind) noexcept{
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
bool cpp_type_is_volatile(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKVolatile>(__match_subject)) {
const cpp_tokens::CKVolatile& cKVolatile = std::get<cpp_tokens::CKVolatile>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_unsigned(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKUnsigned>(__match_subject)) {
const cpp_tokens::CKUnsigned& cKUnsigned = std::get<cpp_tokens::CKUnsigned>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_signed(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKSigned>(__match_subject)) {
const cpp_tokens::CKSigned& cKSigned = std::get<cpp_tokens::CKSigned>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_long(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKLong>(__match_subject)) {
const cpp_tokens::CKLong& cKLong = std::get<cpp_tokens::CKLong>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_short(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKShort>(__match_subject)) {
const cpp_tokens::CKShort& cKShort = std::get<cpp_tokens::CKShort>(__match_subject);
return true;
}
return false;
std::abort();
}();
}
bool cpp_type_is_scope(cpp_tokens::CppTokenKind kind) noexcept{
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
bool cpp_type_is_left_angle(cpp_tokens::CppTokenKind kind) noexcept{
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
bool cpp_type_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept{
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
bool cpp_type_is_star(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String("*", 1));
}
return false;
std::abort();
}();
}
bool cpp_type_is_ampersand(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> bool {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::COp>(__match_subject)) {
const cpp_tokens::COp& cOp = std::get<cpp_tokens::COp>(__match_subject);
auto [symbol] = cOp; return (symbol == mlc::String("&", 1));
}
return false;
std::abort();
}();
}
bool cpp_type_is_rvalue_reference(cpp_tokens::CppTokenKind kind) noexcept{
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
bool cpp_type_is_comma(cpp_tokens::CppTokenKind kind) noexcept{
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
mlc::String cpp_type_spelling_from_kind(cpp_tokens::CppTokenKind kind) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = kind;
if (std::holds_alternative<cpp_tokens::CKVoid>(__match_subject)) {
const cpp_tokens::CKVoid& cKVoid = std::get<cpp_tokens::CKVoid>(__match_subject);
return mlc::String("void", 4);
}
if (std::holds_alternative<cpp_tokens::CKInt>(__match_subject)) {
const cpp_tokens::CKInt& cKInt = std::get<cpp_tokens::CKInt>(__match_subject);
return mlc::String("int", 3);
}
if (std::holds_alternative<cpp_tokens::CKBool>(__match_subject)) {
const cpp_tokens::CKBool& cKBool = std::get<cpp_tokens::CKBool>(__match_subject);
return mlc::String("bool", 4);
}
if (std::holds_alternative<cpp_tokens::CKChar>(__match_subject)) {
const cpp_tokens::CKChar& cKChar = std::get<cpp_tokens::CKChar>(__match_subject);
return mlc::String("char", 4);
}
if (std::holds_alternative<cpp_tokens::CKFloat>(__match_subject)) {
const cpp_tokens::CKFloat& cKFloat = std::get<cpp_tokens::CKFloat>(__match_subject);
return mlc::String("float", 5);
}
if (std::holds_alternative<cpp_tokens::CKDouble>(__match_subject)) {
const cpp_tokens::CKDouble& cKDouble = std::get<cpp_tokens::CKDouble>(__match_subject);
return mlc::String("double", 6);
}
if (std::holds_alternative<cpp_tokens::CKShort>(__match_subject)) {
const cpp_tokens::CKShort& cKShort = std::get<cpp_tokens::CKShort>(__match_subject);
return mlc::String("short", 5);
}
if (std::holds_alternative<cpp_tokens::CKLong>(__match_subject)) {
const cpp_tokens::CKLong& cKLong = std::get<cpp_tokens::CKLong>(__match_subject);
return mlc::String("long", 4);
}
if (std::holds_alternative<cpp_tokens::CIdent>(__match_subject)) {
const cpp_tokens::CIdent& cIdent = std::get<cpp_tokens::CIdent>(__match_subject);
auto [name] = cIdent; return name;
}
return mlc::String("", 0);
std::abort();
}();
}
bool cpp_type_is_specifier_start(cpp_tokens::CppTokenKind kind) noexcept{
  return (((((((cpp_type_spelling_from_kind(kind) != mlc::String("", 0)) || cpp_type_is_const(kind)) || cpp_type_is_volatile(kind)) || cpp_type_is_unsigned(kind)) || cpp_type_is_signed(kind)) || cpp_type_is_long(kind)) || cpp_type_is_short(kind));
}
mlc::String cpp_type_append_spelling(mlc::String existing, mlc::String piece) noexcept{
  if ((existing == mlc::String("", 0)))   {
    return piece;
  } else if ((piece == mlc::String("::", 2)))   {
    return (existing + mlc::String("::", 2));
  } else if (((existing.length() >= 2) && (existing.substring(mlc::arith::checked_sub(existing.length(), 2), 2) == mlc::String("::", 2))))   {
    return (existing + piece);
  } else   {
    return ((existing + mlc::String(" ", 1)) + piece);
  }
}
CppQualifiedNameResult cpp_type_join_qualified_name(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto scan_position = position;
  auto qualified_name = mlc::String("", 0);
  auto after_scope = false;
  while ((!cpp_type_at_end(tokens, scan_position)))   {
    auto kind = cpp_type_token_kind_at(tokens, scan_position);
    if (cpp_type_is_scope(kind))     {
      (qualified_name = cpp_type_append_spelling(qualified_name, mlc::String("::", 2)));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      (after_scope = true);
    } else     {
      auto spelling = cpp_type_spelling_from_kind(kind);
      if ((spelling == mlc::String("", 0)))       {
        break;
      }
      if (((qualified_name != mlc::String("", 0)) && (!after_scope)))       {
        break;
      }
      (qualified_name = cpp_type_append_spelling(qualified_name, spelling));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      (after_scope = false);
    }
  }
  return CppQualifiedNameResult{qualified_name, scan_position};
}
CppTypeQualifierScan cpp_type_parse_qualifiers(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto has_const = false;
  auto prefix_words = mlc::Array<mlc::String>{};
  auto scan_position = position;
  while ((!cpp_type_at_end(tokens, scan_position)))   {
    auto kind = cpp_type_token_kind_at(tokens, scan_position);
    if (cpp_type_is_const(kind))     {
      (has_const = true);
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_volatile(kind))     {
      prefix_words.push_back(mlc::String("volatile", 8));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_unsigned(kind))     {
      prefix_words.push_back(mlc::String("unsigned", 8));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_signed(kind))     {
      prefix_words.push_back(mlc::String("signed", 6));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_long(kind))     {
      prefix_words.push_back(mlc::String("long", 4));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_short(kind))     {
      prefix_words.push_back(mlc::String("short", 5));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else     {
      break;
    }
  }
  return CppTypeQualifierScan{has_const, prefix_words.join(mlc::String(" ", 1)), scan_position};
}
mlc::String cpp_type_full_spelling(mlc::String name_prefix, mlc::String core_name) noexcept{
  if ((name_prefix == mlc::String("", 0)))   {
    return core_name;
  } else if ((core_name == mlc::String("", 0)))   {
    return name_prefix;
  } else   {
    return ((name_prefix + mlc::String(" ", 1)) + core_name);
  }
}
CppTemplateArgumentsResult cpp_type_parse_template_arguments(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto arguments = mlc::Array<std::shared_ptr<cpp_ast::CppType>>{};
  auto scan_position = position;
  if ((!cpp_type_is_left_angle(cpp_type_token_kind_at(tokens, scan_position))))   {
    return CppTemplateArgumentsResult{arguments, scan_position};
  } else   {
    (scan_position = mlc::arith::checked_add(scan_position, 1));
    while (((!cpp_type_at_end(tokens, scan_position)) && (!cpp_type_is_right_angle(cpp_type_token_kind_at(tokens, scan_position)))))     {
      auto argument_parsed = parse_cpp_type(tokens, scan_position);
      arguments.push_back(argument_parsed.type_node);
      (scan_position = argument_parsed.position);
      if (cpp_type_is_comma(cpp_type_token_kind_at(tokens, scan_position)))       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    }
    if (cpp_type_is_right_angle(cpp_type_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppTemplateArgumentsResult{arguments, scan_position};
  }
}
CppTypedParseResult cpp_type_parse_base(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String name_prefix) noexcept{
  auto name_parsed = cpp_type_join_qualified_name(tokens, position);
  auto full_name = cpp_type_full_spelling(name_prefix, name_parsed.name);
  auto template_parsed = cpp_type_parse_template_arguments(tokens, name_parsed.position);
  auto base_type = ((template_parsed.arguments.length() > 0) ? (std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeTemplate{full_name, template_parsed.arguments})) : (std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{full_name})));
  return CppTypedParseResult{base_type, template_parsed.position};
}
CppTypedParseResult cpp_type_parse_suffixes(mlc::Array<cpp_tokens::CppToken> tokens, int position, std::shared_ptr<cpp_ast::CppType> type_node) noexcept{
  auto scan_position = position;
  auto current_type = type_node;
  while ((!cpp_type_at_end(tokens, scan_position)))   {
    auto kind = cpp_type_token_kind_at(tokens, scan_position);
    if (cpp_type_is_star(kind))     {
      (current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypePtr{current_type}));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_rvalue_reference(kind))     {
      (current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRRef{current_type}));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else if (cpp_type_is_ampersand(kind))     {
      (current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRef{current_type}));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else     {
      break;
    }
  }
  return CppTypedParseResult{current_type, scan_position};
}
CppTypedParseResult parse_cpp_type(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto qualifier_scan = cpp_type_parse_qualifiers(tokens, position);
  auto base_parsed = cpp_type_parse_base(tokens, qualifier_scan.position, qualifier_scan.name_prefix);
  auto type_node = base_parsed.type_node;
  auto with_const = (qualifier_scan.has_const ? (std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeConst{type_node})) : (type_node));
  return cpp_type_parse_suffixes(tokens, base_parsed.position, with_const);
}
CppTypeStringResult parse_cpp_type_to_string(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto parsed = parse_cpp_type(tokens, position);
  return CppTypeStringResult{print::print_cpp_type(parsed.type_node), parsed.position};
}

} // namespace cpp_types
