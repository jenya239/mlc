#include "cpp_decls.hpp"

#include "cpp_types.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"

namespace cpp_decls {

using namespace cpp_types;
using namespace cpp_ast;
using namespace cpp_tokens;
using namespace cpp_predicates;

struct CppRecordFieldsParse {mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields;int position;};

struct CppParameterListParse {mlc::Array<mlc::String> parameters;int position;};

struct CppEnumArmsParse {mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms;int position;};

struct CppDeclarationsBlockParse {mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations;int position;mlc::Array<mlc::String> errors;};

struct CppClassMembersParse {mlc::Array<cpp_ast::CppClassMember> members;int position;};

struct CppTemplateHeaderParse {mlc::String parameters;int position;};

cpp_decls::CppDeclarationParseResult cpp_decl_empty_result(int position) noexcept;

int cpp_decl_skip_token(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_skip_to_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_after_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

bool cpp_decl_has_attribute_start(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_skip_one_attribute(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_skip_attributes(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_template_depth_after_token(int depth, cpp_tokens::CppTokenKind kind) noexcept;

int cpp_decl_template_skip_brackets(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, int depth) noexcept;

mlc::String cpp_decl_template_parameters_between(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;

cpp_decls::CppTemplateHeaderParse cpp_decl_parse_template_header(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppClassMembersParse cpp_decl_parse_class_members(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_parse_include_skip(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_include_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_using_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppRecordFieldsParse cpp_decl_parse_record_fields(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult cpp_decl_parse_record_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String kind_word) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_struct_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_class_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppParameterListParse cpp_decl_parse_parameter_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_fn_proto_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppEnumArmsParse cpp_decl_parse_enum_arms(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_enum_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationsBlockParse parse_cpp_declarations_until_right_brace(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_namespace_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

bool cpp_decl_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

bool cpp_decl_can_start_fn_proto(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

bool cpp_decl_can_start_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

int cpp_decl_last_identifier_before_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int start) noexcept;

std::shared_ptr<cpp_ast::CppType> cpp_decl_type_node_until(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_typedef_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_template_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_extern_block_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult parse_cpp_declaration(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

cpp_decls::CppDeclarationParseResult cpp_decl_empty_result(int position) noexcept{return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, mlc::String("__skip__"))), position, mlc::String("")};}

int cpp_decl_skip_token(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_predicates::cpp_at_end(tokens, position) ? position : position + 1;}

int cpp_decl_skip_to_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int scan_position = position;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
return cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) ? scan_position + 1 : scan_position;
}

int cpp_decl_after_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, position)) ? position + 1 : position;}

bool cpp_decl_has_attribute_start(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_predicates::cpp_is_left_bracket(cpp_predicates::cpp_token_kind_at(tokens, position)) && cpp_predicates::cpp_is_left_bracket(cpp_predicates::cpp_token_kind_at(tokens, position + 1));}

int cpp_decl_skip_one_attribute(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
return !cpp_decl_has_attribute_start(tokens, position) ? position : [&]() -> int { 
  int scan_position = position + 2;
  bool found_end = false;
  while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !found_end){
{
if (cpp_predicates::cpp_is_right_bracket(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) && cpp_predicates::cpp_is_right_bracket(cpp_predicates::cpp_token_kind_at(tokens, scan_position + 1))){
{
scan_position = scan_position + 2;
found_end = true;
}
} else {
{
scan_position = scan_position + 1;
}
}
}
}
  return scan_position;
 }();
}

int cpp_decl_skip_attributes(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int scan_position = position;
bool continue_loop = true;
while (continue_loop){
{
int next_position = cpp_decl_skip_one_attribute(tokens, scan_position);
if (next_position == scan_position){
{
continue_loop = false;
}
} else {
{
scan_position = next_position;
}
}
}
}
return scan_position;
}

int cpp_decl_template_depth_after_token(int depth, cpp_tokens::CppTokenKind kind) noexcept{return cpp_predicates::cpp_is_right_angle(kind) ? depth - 1 : cpp_predicates::cpp_is_angle(kind) ? depth + 1 : depth;}

int cpp_decl_template_skip_brackets(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, int depth) noexcept{return cpp_predicates::cpp_at_end(tokens, scan_position) ? scan_position : depth <= 0 ? scan_position : [&]() -> int { 
  cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, scan_position);
  int next_depth = cpp_decl_template_depth_after_token(depth, kind);
  return next_depth <= 0 ? scan_position : cpp_decl_template_skip_brackets(tokens, scan_position + 1, next_depth);
 }();}

mlc::String cpp_decl_template_parameters_between(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept{
mlc::Array<mlc::String> parts = {};
int scan_position = start;
while (scan_position < end_position){
{
mlc::String part = cpp_predicates::cpp_type_part_string(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
if (part != mlc::String("")){
{
parts.push_back(part);
}
}
scan_position = scan_position + 1;
}
}
return parts.join(mlc::String(" "));
}

cpp_decls::CppTemplateHeaderParse cpp_decl_parse_template_header(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_template = position + 1;
return !cpp_predicates::cpp_is_angle(cpp_predicates::cpp_token_kind_at(tokens, after_template)) ? cpp_decls::CppTemplateHeaderParse{mlc::String(""), after_template} : [&]() -> cpp_decls::CppTemplateHeaderParse { 
  int parameter_start = after_template + 1;
  int parameter_end = cpp_decl_template_skip_brackets(tokens, parameter_start, 1);
  int scan_position = parameter_end;
  if (cpp_predicates::cpp_is_right_angle(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppTemplateHeaderParse{cpp_decl_template_parameters_between(tokens, parameter_start, parameter_end), scan_position};
 }();
}

cpp_decls::CppClassMembersParse cpp_decl_parse_class_members(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<cpp_ast::CppClassMember> members = {};
int scan_position = position;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = cpp_decl_skip_attributes(tokens, scan_position);
if (cpp_predicates::cpp_is_public_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
members.push_back(cpp_ast::CppClassMemberAccess((cpp_ast::CppPublic{})));
scan_position = scan_position + 1;
if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
scan_position = scan_position + 1;
}
}
} else {
{
if (cpp_predicates::cpp_is_protected_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
members.push_back(cpp_ast::CppClassMemberAccess((cpp_ast::CppProtected{})));
scan_position = scan_position + 1;
if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
scan_position = scan_position + 1;
}
} else {
if (cpp_predicates::cpp_is_private_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
members.push_back(cpp_ast::CppClassMemberAccess((cpp_ast::CppPrivate{})));
scan_position = scan_position + 1;
if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
scan_position = scan_position + 1;
}
} else {
cpp_types::CppTypedParseResult type_parsed = cpp_types::parse_cpp_type(tokens, scan_position);
mlc::String field_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
if (field_name == mlc::String("")){
scan_position = cpp_decl_skip_token(tokens, scan_position);
} else {
members.push_back(cpp_ast::CppClassMemberField(type_parsed.type_node, field_name, mlc::String("")));
scan_position = cpp_decl_after_semicolon(tokens, type_parsed.position + 1);
}
}
}
}
}
}
}
return cpp_decls::CppClassMembersParse{members, scan_position};
}

int cpp_decl_parse_include_skip(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int scan_position = position;
if (cpp_predicates::cpp_is_hash(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
if (cpp_predicates::cpp_is_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) && cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) == mlc::String("include")){
{
scan_position = scan_position + 1;
}
}
return scan_position;
}

cpp_decls::CppDeclarationParseResult parse_cpp_include_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_skip = cpp_decl_parse_include_skip(tokens, position);
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, after_skip);
return cpp_predicates::cpp_is_angle(kind) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  mlc::String name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_skip + 1));
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(true, name)), after_skip + 3, mlc::String("")};
 }() : cpp_predicates::cpp_is_string(kind) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  mlc::String path = cpp_predicates::cpp_get_string(kind);
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude(false, path)), after_skip + 1, mlc::String("")};
 }() : cpp_decl_empty_result(after_skip);
}

cpp_decls::CppDeclarationParseResult parse_cpp_using_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_using = position + 1;
mlc::String alias = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_using));
int after_alias = after_using + 1;
int after_equals = cpp_predicates::cpp_is_equals(cpp_predicates::cpp_token_kind_at(tokens, after_alias)) ? after_alias + 1 : after_alias;
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, after_equals);
int after_type = type_parsed.position;
return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppUsing(alias, type_parsed.type_string)), cpp_decl_after_semicolon(tokens, after_type), mlc::String("")};
}

cpp_decls::CppRecordFieldsParse cpp_decl_parse_record_fields(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields = {};
int scan_position = position;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
cpp_types::CppTypedParseResult type_parsed = cpp_types::parse_cpp_type(tokens, scan_position);
mlc::String field_type = print::print_cpp_type(type_parsed.type_node);
scan_position = type_parsed.position;
mlc::String field_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
scan_position = scan_position + 1;
scan_position = cpp_decl_after_semicolon(tokens, scan_position);
fields.push_back(std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{field_type, field_name}));
}
}
return cpp_decls::CppRecordFieldsParse{fields, scan_position};
}

cpp_decls::CppDeclarationParseResult cpp_decl_parse_record_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String kind_word) noexcept{
bool is_struct = kind_word == mlc::String("struct");
int after_keyword = position + 1;
mlc::String name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_keyword));
int after_name = after_keyword + 1;
return cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  cpp_decls::CppClassMembersParse members_parsed = cpp_decl_parse_class_members(tokens, after_name + 1);
  int scan_position = members_parsed.position;
  if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppClassDeclaration(cpp_ast::CppClassDefinition{is_struct, mlc::String(""), name, {}, members_parsed.members})), cpp_decl_after_semicolon(tokens, scan_position), mlc::String("")};
 }() : cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, after_name)) ? cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl(kind_word, name)), after_name + 1, mlc::String("")} : cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct(mlc::String(""), name, {}, true)), after_name, mlc::String("")};
}

cpp_decls::CppDeclarationParseResult parse_cpp_struct_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_decl_parse_record_decl(tokens, position, mlc::String("struct"));}

cpp_decls::CppDeclarationParseResult parse_cpp_class_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{return cpp_decl_parse_record_decl(tokens, position, mlc::String("class"));}

cpp_decls::CppParameterListParse cpp_decl_parse_parameter_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<mlc::String> parameters = {};
int scan_position = position;
return !cpp_predicates::cpp_is_left_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) ? cpp_decls::CppParameterListParse{parameters, scan_position} : [&]() -> cpp_decls::CppParameterListParse { 
  scan_position = scan_position + 1;
  while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, scan_position);
scan_position = type_parsed.position;
mlc::String parameter_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
if (parameter_name != mlc::String("")){
{
parameters.push_back(type_parsed.type_string + mlc::String(" ") + parameter_name);
scan_position = scan_position + 1;
}
} else {
{
parameters.push_back(type_parsed.type_string);
}
}
if (cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
}
}
  if (cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppParameterListParse{parameters, scan_position};
 }();
}

cpp_decls::CppDeclarationParseResult parse_cpp_fn_proto_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
mlc::String function_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
return function_name == mlc::String("") ? cpp_decl_empty_result(type_parsed.position) : [&]() -> cpp_decls::CppDeclarationParseResult { 
  cpp_decls::CppParameterListParse parameters_parsed = cpp_decl_parse_parameter_list(tokens, type_parsed.position + 1);
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto(mlc::String(""), type_parsed.type_string, function_name, parameters_parsed.parameters)), cpp_decl_after_semicolon(tokens, parameters_parsed.position), mlc::String("")};
 }();
}

cpp_decls::CppEnumArmsParse cpp_decl_parse_enum_arms(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms = {};
int scan_position = position;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
mlc::String arm_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
if (arm_name != mlc::String("")){
{
arms.push_back(std::make_shared<cpp_ast::CppVariantArm>(cpp_ast::CppVariantArm{arm_name, {}}));
scan_position = scan_position + 1;
}
} else {
{
scan_position = cpp_decl_skip_token(tokens, scan_position);
}
}
if (cpp_predicates::cpp_is_equals(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
[&]() { 
  while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) && !cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
 }();
}
}
if (cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
}
}
return cpp_decls::CppEnumArmsParse{arms, scan_position};
}

cpp_decls::CppDeclarationParseResult parse_cpp_enum_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_enum = position + 1;
mlc::String enum_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_enum));
int after_name = after_enum + 1;
return cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  cpp_decls::CppEnumArmsParse arms_parsed = cpp_decl_parse_enum_arms(tokens, after_name + 1);
  int scan_position = arms_parsed.position;
  if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppVariant(mlc::String(""), enum_name, arms_parsed.arms)), cpp_decl_after_semicolon(tokens, scan_position), mlc::String("")};
 }() : cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, after_name)) ? cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl(mlc::String("enum"), enum_name)), after_name + 1, mlc::String("")} : cpp_decl_empty_result(after_name);
}

cpp_decls::CppDeclarationsBlockParse parse_cpp_declarations_until_right_brace(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations = {};
mlc::Array<mlc::String> errors = {};
int scan_position = position;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = cpp_decl_skip_attributes(tokens, scan_position);
cpp_decls::CppDeclarationParseResult parsed = parse_cpp_declaration(tokens, scan_position);
if (parsed.error_message != mlc::String("")){
{
errors.push_back(parsed.error_message);
}
}
if (!cpp_decl_is_skip(parsed.declaration)){
{
declarations.push_back(parsed.declaration);
}
}
if (parsed.position == scan_position){
{
scan_position = cpp_decl_skip_token(tokens, scan_position);
}
} else {
{
scan_position = parsed.position;
}
}
}
}
return cpp_decls::CppDeclarationsBlockParse{declarations, scan_position, errors};
}

cpp_decls::CppDeclarationParseResult parse_cpp_namespace_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_namespace = position + 1;
mlc::String namespace_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_namespace));
int after_name = after_namespace + 1;
return cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  cpp_decls::CppDeclarationsBlockParse inner = parse_cpp_declarations_until_right_brace(tokens, after_name + 1);
  int scan_position = inner.position;
  if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppNamespace(namespace_name, inner.declarations)), scan_position, inner.errors.join(mlc::String("; "))};
 }() : cpp_decl_empty_result(after_name);
}

bool cpp_decl_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() { if (std::holds_alternative<cpp_ast::CppInclude>((*declaration))) { auto _v_cppinclude = std::get<cpp_ast::CppInclude>((*declaration)); auto [is_angle, path] = _v_cppinclude; return is_angle && path == mlc::String("__skip__"); } return false; }();}

bool cpp_decl_can_start_fn_proto(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
int name_position = type_parsed.position;
mlc::String function_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, name_position));
return function_name != mlc::String("") && cpp_predicates::cpp_is_left_paren(cpp_predicates::cpp_token_kind_at(tokens, name_position + 1));
}

bool cpp_decl_can_start_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
int name_position = type_parsed.position;
mlc::String variable_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, name_position));
return variable_name != mlc::String("") && cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, name_position + 1));
}

cpp_decls::CppDeclarationParseResult parse_cpp_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_types::CppTypeStringResult type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
mlc::String variable_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
int after_name = type_parsed.position + 1;
return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationFragment(type_parsed.type_string + mlc::String(" ") + variable_name + mlc::String(";"))), cpp_decl_after_semicolon(tokens, after_name), mlc::String("")};
}

int cpp_decl_last_identifier_before_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int start) noexcept{
int scan_position = start;
int last_identifier_position = start;
while (!cpp_predicates::cpp_at_end(tokens, scan_position) && !cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
if (cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) != mlc::String("")){
{
last_identifier_position = scan_position;
}
}
scan_position = scan_position + 1;
}
}
return last_identifier_position;
}

std::shared_ptr<cpp_ast::CppType> cpp_decl_type_node_until(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept{
mlc::Array<mlc::String> parts = {};
int scan_position = start;
while (scan_position < end_position){
{
mlc::String part = cpp_predicates::cpp_type_part_string(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
if (part != mlc::String("")){
{
parts.push_back(part);
}
}
scan_position = scan_position + 1;
}
}
return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName(parts.join(mlc::String(" "))));
}

cpp_decls::CppDeclarationParseResult parse_cpp_typedef_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_typedef = position + 1;
int alias_position = cpp_decl_last_identifier_before_semicolon(tokens, after_typedef);
mlc::String alias = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, alias_position));
return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppTypedefDeclaration(alias, cpp_decl_type_node_until(tokens, after_typedef, alias_position))), cpp_decl_after_semicolon(tokens, alias_position + 1), mlc::String("")};
}

cpp_decls::CppDeclarationParseResult parse_cpp_template_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
cpp_decls::CppTemplateHeaderParse header = cpp_decl_parse_template_header(tokens, position);
cpp_decls::CppDeclarationParseResult inner = parse_cpp_declaration(tokens, cpp_decl_skip_attributes(tokens, header.position));
return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppTemplateDeclaration(header.parameters, inner.declaration)), inner.position, inner.error_message};
}

cpp_decls::CppDeclarationParseResult parse_cpp_extern_block_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int after_extern = position + 1;
mlc::String linkage = cpp_predicates::cpp_is_string(cpp_predicates::cpp_token_kind_at(tokens, after_extern)) ? mlc::String("\"") + cpp_predicates::cpp_get_string(cpp_predicates::cpp_token_kind_at(tokens, after_extern)) + mlc::String("\"") : mlc::String("");
int after_linkage = linkage != mlc::String("") ? after_extern + 1 : after_extern;
return cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_linkage)) ? [&]() -> cpp_decls::CppDeclarationParseResult { 
  cpp_decls::CppDeclarationsBlockParse inner = parse_cpp_declarations_until_right_brace(tokens, after_linkage + 1);
  int scan_position = inner.position;
  if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))){
{
scan_position = scan_position + 1;
}
}
  return cpp_decls::CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppExternBlock(linkage, inner.declarations)), scan_position, inner.errors.join(mlc::String("; "))};
 }() : cpp_decl_empty_result(after_linkage);
}

cpp_decls::CppDeclarationParseResult parse_cpp_declaration(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
int start_position = cpp_decl_skip_attributes(tokens, position);
cpp_tokens::CppTokenKind kind = cpp_predicates::cpp_token_kind_at(tokens, start_position);
return cpp_predicates::cpp_is_hash(kind) ? parse_cpp_include_decl(tokens, start_position) : cpp_predicates::cpp_is_template_keyword(kind) ? parse_cpp_template_decl(tokens, start_position) : cpp_predicates::cpp_is_extern_keyword(kind) ? parse_cpp_extern_block_decl(tokens, start_position) : cpp_predicates::cpp_is_using_keyword(kind) ? parse_cpp_using_decl(tokens, start_position) : cpp_predicates::cpp_is_typedef_keyword(kind) ? parse_cpp_typedef_decl(tokens, start_position) : cpp_predicates::cpp_is_struct_keyword(kind) ? parse_cpp_struct_decl(tokens, start_position) : cpp_predicates::cpp_is_class_keyword(kind) ? parse_cpp_class_decl(tokens, start_position) : cpp_predicates::cpp_is_enum_keyword(kind) ? parse_cpp_enum_decl(tokens, start_position) : cpp_predicates::cpp_is_namespace_keyword(kind) ? parse_cpp_namespace_decl(tokens, start_position) : cpp_decl_can_start_fn_proto(tokens, start_position) ? parse_cpp_fn_proto_decl(tokens, start_position) : cpp_decl_can_start_variable_decl(tokens, start_position) ? parse_cpp_variable_decl(tokens, start_position) : [&]() -> cpp_decls::CppDeclarationParseResult { 
  int recovered_position = cpp_decl_skip_to_semicolon(tokens, start_position);
  return cpp_decls::CppDeclarationParseResult{cpp_decl_empty_result(start_position).declaration, recovered_position, mlc::String("unrecognized declaration")};
 }();
}

} // namespace cpp_decls
