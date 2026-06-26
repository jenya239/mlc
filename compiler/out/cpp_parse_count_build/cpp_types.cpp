#include "cpp_types.hpp"

#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"

namespace cpp_types {

using namespace cpp_ast;
using namespace cpp_tokens;

struct CppTypeQualifierScan {bool has_const;mlc::String name_prefix;int position;};

struct CppQualifiedNameResult {mlc::String name;int position;};

struct CppTemplateArgumentsResult {mlc::Array<std::shared_ptr<cpp_ast::CppType>> arguments;int position;};

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

cpp_types::CppQualifiedNameResult cpp_type_join_qualified_name(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_types::CppTypeQualifierScan cpp_type_parse_qualifiers(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

mlc::String cpp_type_full_spelling(mlc::String name_prefix, mlc::String core_name) noexcept;

cpp_types::CppTemplateArgumentsResult cpp_type_parse_template_arguments(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_types::CppTypedParseResult cpp_type_parse_base(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String name_prefix) noexcept;

cpp_types::CppTypedParseResult cpp_type_parse_suffixes(mlc::Array<cpp_tokens::CppToken> tokens, int position, std::shared_ptr<cpp_ast::CppType> type_node) noexcept;

cpp_types::CppTypedParseResult parse_cpp_type(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_types::CppTypeStringResult parse_cpp_type_to_string(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_tokens::CppTokenKind cpp_type_token_kind_at(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return position < tokens.size() ? cpp_tokens::CppTokenKind(tokens[position].kind) : cpp_tokens::CppTokenKind((cpp_tokens::CEof{}));}

bool cpp_type_at_end(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CEof>(cpp_type_token_kind_at(tokens, position))) {  return true; } return false; }();}

bool cpp_type_is_const(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKConst>(kind)) {  return true; } return false; }();}

bool cpp_type_is_volatile(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKVolatile>(kind)) {  return true; } return false; }();}

bool cpp_type_is_unsigned(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKUnsigned>(kind)) {  return true; } return false; }();}

bool cpp_type_is_signed(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKSigned>(kind)) {  return true; } return false; }();}

bool cpp_type_is_long(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKLong>(kind)) {  return true; } return false; }();}

bool cpp_type_is_short(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CKShort>(kind)) {  return true; } return false; }();}

bool cpp_type_is_scope(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CScope>(kind)) {  return true; } return false; }();}

bool cpp_type_is_left_angle(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CLAngle>(kind)) {  return true; } return false; }();}

bool cpp_type_is_right_angle(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CRAngle>(kind)) {  return true; } return false; }();}

bool cpp_type_is_star(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String("*"); } return false; }();}

bool cpp_type_is_ampersand(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String("&"); } return false; }();}

bool cpp_type_is_rvalue_reference(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::CAnd>(kind)) {  return true; } return false; }();}

bool cpp_type_is_comma(cpp_tokens::CppTokenKind kind) noexcept{return [&]() { if (std::holds_alternative<cpp_tokens::COp>(kind)) { auto _v_cop = std::get<cpp_tokens::COp>(kind); auto [symbol] = _v_cop; return symbol == mlc::String(","); } return false; }();}

mlc::String cpp_type_spelling_from_kind(cpp_tokens::CppTokenKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<cpp_tokens::CKVoid>(kind)) {  return mlc::String("void"); } if (std::holds_alternative<cpp_tokens::CKInt>(kind)) {  return mlc::String("int"); } if (std::holds_alternative<cpp_tokens::CKBool>(kind)) {  return mlc::String("bool"); } if (std::holds_alternative<cpp_tokens::CKChar>(kind)) {  return mlc::String("char"); } if (std::holds_alternative<cpp_tokens::CKFloat>(kind)) {  return mlc::String("float"); } if (std::holds_alternative<cpp_tokens::CKDouble>(kind)) {  return mlc::String("double"); } if (std::holds_alternative<cpp_tokens::CKShort>(kind)) {  return mlc::String("short"); } if (std::holds_alternative<cpp_tokens::CKLong>(kind)) {  return mlc::String("long"); } if (std::holds_alternative<cpp_tokens::CIdent>(kind)) { auto _v_cident = std::get<cpp_tokens::CIdent>(kind); auto [name] = _v_cident; return name; } return mlc::String(""); }();}

bool cpp_type_is_specifier_start(cpp_tokens::CppTokenKind kind) noexcept{return cpp_type_spelling_from_kind(kind) != mlc::String("") || cpp_type_is_const(kind) || cpp_type_is_volatile(kind) || cpp_type_is_unsigned(kind) || cpp_type_is_signed(kind) || cpp_type_is_long(kind) || cpp_type_is_short(kind);}

mlc::String cpp_type_append_spelling(mlc::String existing, mlc::String piece) noexcept{return existing == mlc::String("") ? piece : piece == mlc::String("::") ? existing + mlc::String("::") : existing.length() >= 2 && existing.substring(existing.length() - 2, 2) == mlc::String("::") ? existing + piece : existing + mlc::String(" ") + piece;}

cpp_types::CppQualifiedNameResult cpp_type_join_qualified_name(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int scan_position = position;
mlc::String qualified_name = mlc::String("");
bool after_scope = false;
while (!cpp_type_at_end(tokens, scan_position)){
{
cpp_tokens::CppTokenKind kind = cpp_type_token_kind_at(tokens, scan_position);
if (cpp_type_is_scope(kind)){
{
qualified_name = cpp_type_append_spelling(qualified_name, mlc::String("::"));
scan_position = scan_position + 1;
after_scope = true;
}
} else {
{
mlc::String spelling = cpp_type_spelling_from_kind(kind);
if (spelling == mlc::String("")){
{
break;
}
}
if (qualified_name != mlc::String("") && !after_scope){
{
break;
}
}
qualified_name = cpp_type_append_spelling(qualified_name, spelling);
scan_position = scan_position + 1;
after_scope = false;
}
}
}
}
return cpp_types::CppQualifiedNameResult{qualified_name, scan_position};
}

cpp_types::CppTypeQualifierScan cpp_type_parse_qualifiers(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
bool has_const = false;
mlc::Array<mlc::String> prefix_words = {};
int scan_position = position;
while (!cpp_type_at_end(tokens, scan_position)){
{
cpp_tokens::CppTokenKind kind = cpp_type_token_kind_at(tokens, scan_position);
if (cpp_type_is_const(kind)){
{
has_const = true;
scan_position = scan_position + 1;
}
} else {
{
if (cpp_type_is_volatile(kind)){
prefix_words.push_back(mlc::String("volatile"));
scan_position = scan_position + 1;
} else {
if (cpp_type_is_unsigned(kind)){
prefix_words.push_back(mlc::String("unsigned"));
scan_position = scan_position + 1;
} else {
if (cpp_type_is_signed(kind)){
prefix_words.push_back(mlc::String("signed"));
scan_position = scan_position + 1;
} else {
if (cpp_type_is_long(kind)){
prefix_words.push_back(mlc::String("long"));
scan_position = scan_position + 1;
} else {
if (cpp_type_is_short(kind)){
prefix_words.push_back(mlc::String("short"));
scan_position = scan_position + 1;
} else {
break;
}
}
}
}
}
}
}
}
}
return cpp_types::CppTypeQualifierScan{has_const, prefix_words.join(mlc::String(" ")), scan_position};
}

mlc::String cpp_type_full_spelling(mlc::String name_prefix, mlc::String core_name) noexcept{return name_prefix == mlc::String("") ? core_name : core_name == mlc::String("") ? name_prefix : name_prefix + mlc::String(" ") + core_name;}

cpp_types::CppTemplateArgumentsResult cpp_type_parse_template_arguments(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppType>> arguments = {};
int scan_position = position;
return !cpp_type_is_left_angle(cpp_type_token_kind_at(tokens, scan_position)) ? cpp_types::CppTemplateArgumentsResult{arguments, scan_position} : [&]() -> cpp_types::CppTemplateArgumentsResult { 
  scan_position = scan_position + 1;
  while (!cpp_type_at_end(tokens, scan_position) && !cpp_type_is_right_angle(cpp_type_token_kind_at(tokens, scan_position))){
{
cpp_types::CppTypedParseResult argument_parsed = parse_cpp_type(tokens, scan_position);
arguments.push_back(argument_parsed.type_node);
scan_position = argument_parsed.position;
if (cpp_type_is_comma(cpp_type_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
}
}
  if (cpp_type_is_right_angle(cpp_type_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_types::CppTemplateArgumentsResult{arguments, scan_position};
 }();
}

cpp_types::CppTypedParseResult cpp_type_parse_base(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String name_prefix) noexcept{
cpp_types::CppQualifiedNameResult name_parsed = cpp_type_join_qualified_name(tokens, position);
mlc::String full_name = cpp_type_full_spelling(name_prefix, name_parsed.name);
cpp_types::CppTemplateArgumentsResult template_parsed = cpp_type_parse_template_arguments(tokens, name_parsed.position);
std::shared_ptr<cpp_ast::CppType> base_type = template_parsed.arguments.size() > 0 ? std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeTemplate(full_name, template_parsed.arguments)) : std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(full_name));
return cpp_types::CppTypedParseResult{base_type, template_parsed.position};
}

cpp_types::CppTypedParseResult cpp_type_parse_suffixes(mlc::Array<cpp_tokens::CppToken> tokens, int position, std::shared_ptr<cpp_ast::CppType> type_node) noexcept{
int scan_position = position;
std::shared_ptr<cpp_ast::CppType> current_type = type_node;
while (!cpp_type_at_end(tokens, scan_position)){
{
cpp_tokens::CppTokenKind kind = cpp_type_token_kind_at(tokens, scan_position);
if (cpp_type_is_star(kind)){
{
current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypePtr(current_type));
scan_position = scan_position + 1;
}
} else {
{
if (cpp_type_is_rvalue_reference(kind)){
current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRRef(current_type));
scan_position = scan_position + 1;
} else {
if (cpp_type_is_ampersand(kind)){
current_type = std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeRef(current_type));
scan_position = scan_position + 1;
} else {
break;
}
}
}
}
}
}
return cpp_types::CppTypedParseResult{current_type, scan_position};
}

cpp_types::CppTypedParseResult parse_cpp_type(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypeQualifierScan qualifier_scan = cpp_type_parse_qualifiers(tokens, position);
cpp_types::CppTypedParseResult base_parsed = cpp_type_parse_base(tokens, qualifier_scan.position, qualifier_scan.name_prefix);
std::shared_ptr<cpp_ast::CppType> type_node = base_parsed.type_node;
std::shared_ptr<cpp_ast::CppType> with_const = qualifier_scan.has_const ? std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeConst(type_node)) : type_node;
return cpp_type_parse_suffixes(tokens, base_parsed.position, with_const);
}

cpp_types::CppTypeStringResult parse_cpp_type_to_string(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypedParseResult parsed = parse_cpp_type(tokens, position);
return cpp_types::CppTypeStringResult{cpp_ast::print_cpp_type(parsed.type_node), parsed.position};
}

} // namespace cpp_types
