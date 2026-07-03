#define main mlc_user_main
#include "header_import.hpp"

#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "ast.hpp"
#include "registry.hpp"

namespace header_import {

using namespace cpp_lexer;
using namespace cpp_parser;
using namespace cpp_ast;
using namespace print;
using namespace ast;
using namespace registry;
using namespace ast_tokens;

bool is_cpp_header_path(mlc::String path) noexcept{
  return (((path.length() >= 2) && (path.substring((path.length() - 2), 2) == mlc::String(".h", 2))) || ((path.length() >= 4) && (path.substring((path.length() - 4), 4) == mlc::String(".hpp", 4))));
}
std::shared_ptr<ast::TypeExpr> cpp_type_string_to_type_expr(mlc::String type_string) noexcept{
  return [&]() -> std::shared_ptr<ast::TypeExpr> {
auto __match_subject = type_string;
if ((__match_subject == mlc::String("int", 3))) {
  return std::make_shared<ast::TypeExpr>(ast::TyI32{});
} else if ((__match_subject == mlc::String("bool", 4))) {
  return std::make_shared<ast::TypeExpr>(ast::TyBool{});
} else if ((__match_subject == mlc::String("void", 4))) {
  return std::make_shared<ast::TypeExpr>(ast::TyUnit{});
} else {
  return std::make_shared<ast::TypeExpr>(ast::TyNamed{type_string});
}
}();
}
CppParameterParts split_cpp_parameter_string(mlc::String parameter_string) noexcept{
  auto last_space = (-1);
  auto scan_index = 0;
  while ((scan_index < parameter_string.length()))   {
    if ((parameter_string.char_at(scan_index) == mlc::String(" ", 1)))     {
      (last_space = scan_index);
    }
    (scan_index = (scan_index + 1));
  }
  if ((last_space < 0))   {
    return CppParameterParts{parameter_string, mlc::String("", 0)};
  } else   {
    return CppParameterParts{parameter_string.substring(0, last_space), parameter_string.substring((last_space + 1), ((parameter_string.length() - last_space) - 1))};
  }
}
mlc::Array<std::shared_ptr<ast::Param>> cpp_parameter_strings_to_params(mlc::Array<mlc::String> parameter_strings) noexcept{
  auto parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
  auto index = 0;
  while ((index < parameter_strings.length()))   {
    auto parts = split_cpp_parameter_string(parameter_strings[index]);
    auto parameter_name = ((parts.name == mlc::String("", 0)) ? (((mlc::String("arg", 3) + mlc::to_string(index)) + mlc::String("", 0))) : (parts.name));
    parameters.push_back(std::make_shared<ast::Param>(ast::Param{parameter_name, false, cpp_type_string_to_type_expr(parts.type_string), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pattern>(ast::PatternWild{ast::span_unknown()})}));
    (index = (index + 1));
  }
  return parameters;
}
mlc::Array<std::shared_ptr<ast::Param>> cpp_cpp_parameters_to_mlc_params(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept{
  auto mlc_parameters = mlc::Array<std::shared_ptr<ast::Param>>{};
  auto index = 0;
  while ((index < parameters.length()))   {
    auto parameter = parameters[index];
    auto parameter_name = ((parameter->name == mlc::String("", 0)) ? (((mlc::String("arg", 3) + mlc::to_string(index)) + mlc::String("", 0))) : (parameter->name));
    mlc_parameters.push_back(std::make_shared<ast::Param>(ast::Param{parameter_name, false, cpp_type_string_to_type_expr(print::print_cpp_type(parameter->parameter_type)), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()}), std::make_shared<ast::Pattern>(ast::PatternWild{ast::span_unknown()})}));
    (index = (index + 1));
  }
  return mlc_parameters;
}
mlc::Array<std::shared_ptr<ast::Decl>> cpp_function_prototype_to_mlc_decls(cpp_ast::CppFunctionPrototype prototype) noexcept{
  return mlc::Array{std::make_shared<ast::Decl>(ast::DeclFn{prototype.name, {}, {}, cpp_cpp_parameters_to_mlc_params(prototype.parameters), cpp_type_string_to_type_expr(print::print_cpp_type(prototype.return_type)), std::make_shared<ast::Expr>(ast::ExprExtern{ast::span_unknown()}), {}})};
}
mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_fields_to_field_definitions(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept{
  auto field_definitions = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
  auto index = 0;
  while ((index < fields.length()))   {
    auto field = fields[index];
    field_definitions.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field->name, cpp_type_string_to_type_expr(field->type_value), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})}));
    (index = (index + 1));
  }
  return field_definitions;
}
mlc::Array<std::shared_ptr<ast::TypeVariant>> cpp_enum_arms_to_variants(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept{
  auto variants = mlc::Array<std::shared_ptr<ast::TypeVariant>>{};
  auto index = 0;
  while ((index < arms.length()))   {
    variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarUnit{arms[index]->name, false}));
    (index = (index + 1));
  }
  return variants;
}
mlc::String cpp_class_member_field_name(cpp_ast::CppClassMember member) noexcept{
  return [&]() -> mlc::String {
auto __match_subject = member;
if (std::holds_alternative<cpp_ast::CppClassMemberField>(__match_subject)) {
const cpp_ast::CppClassMemberField& cppClassMemberField = std::get<cpp_ast::CppClassMemberField>(__match_subject);
auto [__0, field_name, __2] = cppClassMemberField; return field_name;
}
return mlc::String("", 0);
std::abort();
}();
}
std::shared_ptr<cpp_ast::CppType> cpp_class_member_field_type(cpp_ast::CppClassMember member) noexcept{
  return [&]() -> std::shared_ptr<cpp_ast::CppType> {
auto __match_subject = member;
if (std::holds_alternative<cpp_ast::CppClassMemberField>(__match_subject)) {
const cpp_ast::CppClassMemberField& cppClassMemberField = std::get<cpp_ast::CppClassMemberField>(__match_subject);
auto [type_node, __1, __2] = cppClassMemberField; return type_node;
}
return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{mlc::String("", 0)});
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_class_members_to_field_definitions(mlc::Array<cpp_ast::CppClassMember> members) noexcept{
  auto field_definitions = mlc::Array<std::shared_ptr<ast::FieldDef>>{};
  auto index = 0;
  while ((index < members.length()))   {
    auto member = members[index];
    auto field_name = cpp_class_member_field_name(member);
    if ((field_name != mlc::String("", 0)))     {
      field_definitions.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field_name, cpp_type_string_to_type_expr(print::print_cpp_type(cpp_class_member_field_type(member))), false, std::make_shared<ast::Expr>(ast::ExprUnit{ast::span_unknown()})}));
    }
    (index = (index + 1));
  }
  return field_definitions;
}
mlc::Array<std::shared_ptr<ast::Decl>> cpp_declaration_to_mlc_decls(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> {
auto __match_subject = declaration;
if (std::holds_alternative<cpp_ast::CppFnProto>((*__match_subject))) {
const cpp_ast::CppFnProto& cppFnProto = std::get<cpp_ast::CppFnProto>((*__match_subject));
auto [__0, return_type, function_name, parameter_strings] = cppFnProto; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclFn{function_name, {}, {}, cpp_parameter_strings_to_params(parameter_strings), cpp_type_string_to_type_expr(return_type), std::make_shared<ast::Expr>(ast::ExprExtern{ast::span_unknown()}), {}})};
}
if (std::holds_alternative<cpp_ast::CppStruct>((*__match_subject))) {
const cpp_ast::CppStruct& cppStruct = std::get<cpp_ast::CppStruct>((*__match_subject));
auto [__0, type_name, fields, __3] = cppStruct; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclType{type_name, {}, mlc::Array{std::make_shared<ast::TypeVariant>(ast::VarRecord{type_name, cpp_fields_to_field_definitions(fields), false})}, {}, ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppClassDeclaration>((*__match_subject))) {
const cpp_ast::CppClassDeclaration& cppClassDeclaration = std::get<cpp_ast::CppClassDeclaration>((*__match_subject));
auto [definition] = cppClassDeclaration; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclType{definition.name, {}, mlc::Array{std::make_shared<ast::TypeVariant>(ast::VarRecord{definition.name, cpp_class_members_to_field_definitions(definition.members), false})}, {}, ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppTypedefDeclaration>((*__match_subject))) {
const cpp_ast::CppTypedefDeclaration& cppTypedefDeclaration = std::get<cpp_ast::CppTypedefDeclaration>((*__match_subject));
auto [alias_name, type_node] = cppTypedefDeclaration; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclTypeAlias{alias_name, {}, cpp_type_string_to_type_expr(print::print_cpp_type(type_node)), ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppTemplateDeclaration>((*__match_subject))) {
const cpp_ast::CppTemplateDeclaration& cppTemplateDeclaration = std::get<cpp_ast::CppTemplateDeclaration>((*__match_subject));
auto [__0, inner_declaration] = cppTemplateDeclaration; return cpp_declaration_to_mlc_decls(inner_declaration);
}
if (std::holds_alternative<cpp_ast::CppExternBlock>((*__match_subject))) {
const cpp_ast::CppExternBlock& cppExternBlock = std::get<cpp_ast::CppExternBlock>((*__match_subject));
auto [__0, inner_declarations] = cppExternBlock; return cpp_declarations_to_mlc_decls_list(inner_declarations);
}
if (std::holds_alternative<cpp_ast::CppFunctionPrototypeDecl>((*__match_subject))) {
const cpp_ast::CppFunctionPrototypeDecl& cppFunctionPrototypeDecl = std::get<cpp_ast::CppFunctionPrototypeDecl>((*__match_subject));
auto [prototype] = cppFunctionPrototypeDecl; return cpp_function_prototype_to_mlc_decls(prototype);
}
if (std::holds_alternative<cpp_ast::CppForwardDecl>((*__match_subject))) {
const cpp_ast::CppForwardDecl& cppForwardDecl = std::get<cpp_ast::CppForwardDecl>((*__match_subject));
auto [__0, type_name] = cppForwardDecl; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclType{type_name, {}, mlc::Array{std::make_shared<ast::TypeVariant>(ast::VarRecord{type_name, {}, false})}, {}, ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppUsing>((*__match_subject))) {
const cpp_ast::CppUsing& cppUsing = std::get<cpp_ast::CppUsing>((*__match_subject));
auto [alias, type_string] = cppUsing; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclTypeAlias{alias, {}, cpp_type_string_to_type_expr(type_string), ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppVariant>((*__match_subject))) {
const cpp_ast::CppVariant& cppVariant = std::get<cpp_ast::CppVariant>((*__match_subject));
auto [__0, enum_name, arms] = cppVariant; return mlc::Array{std::make_shared<ast::Decl>(ast::DeclType{enum_name, {}, cpp_enum_arms_to_variants(arms), {}, ast::span_unknown()})};
}
if (std::holds_alternative<cpp_ast::CppNamespace>((*__match_subject))) {
const cpp_ast::CppNamespace& cppNamespace = std::get<cpp_ast::CppNamespace>((*__match_subject));
auto [__0, inner_declarations] = cppNamespace; return cpp_declarations_to_mlc_decls_list(inner_declarations);
}
return [&]() {
auto empty_decls = mlc::Array<std::shared_ptr<ast::Decl>>{};
return empty_decls;
}();
std::abort();
}();
}
mlc::Array<std::shared_ptr<ast::Decl>> cpp_declarations_to_mlc_decls_list(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
  auto mlc_declarations = mlc::Array<std::shared_ptr<ast::Decl>>{};
  auto index = 0;
  while ((index < declarations.length()))   {
    auto converted = cpp_declaration_to_mlc_decls(declarations[index]);
    auto inner_index = 0;
    while ((inner_index < converted.length()))     {
      mlc_declarations.push_back(converted[inner_index]);
      (inner_index = (inner_index + 1));
    }
    (index = (index + 1));
  }
  return mlc_declarations;
}
HeaderImportResult parse_cpp_header_source(mlc::String source_text) noexcept{
  auto tokenize_result = cpp_lexer::cpp_tokenize(source_text);
  auto parse_result = cpp_parser::cpp_parse(tokenize_result.tokens);
  return HeaderImportResult{cpp_declarations_to_mlc_decls_list(parse_result.program.declarations), parse_result.errors};
}
HeaderImportResult load_cpp_header_decls(mlc::String path) noexcept{
  if ((!mlc::file::exists(path)))   {
    return HeaderImportResult{{}, mlc::Array<mlc::String>{((mlc::String("file not found: ", 16) + mlc::to_string(path)) + mlc::String("", 0))}};
  } else   {
    return parse_cpp_header_source(mlc::file::read_to_string(path));
  }
}
registry::TypeRegistry registry_from_cpp_header_source(mlc::String source_text) noexcept{
  return registry::build_registry(ast::Program{parse_cpp_header_source(source_text).declarations});
}

} // namespace header_import
