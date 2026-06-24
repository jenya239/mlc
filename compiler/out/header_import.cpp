#include "header_import.hpp"

#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include "ast.hpp"
#include "registry.hpp"

namespace header_import {

using namespace cpp_lexer;
using namespace cpp_parser;
using namespace cpp_ast;
using namespace ast;
using namespace registry;
using namespace ast_tokens;

struct CppParameterParts {mlc::String type_string;mlc::String name;};

bool is_cpp_header_path(mlc::String path) noexcept;

std::shared_ptr<ast::TypeExpr> cpp_type_string_to_type_expr(mlc::String type_string) noexcept;

header_import::CppParameterParts split_cpp_parameter_string(mlc::String parameter_string) noexcept;

mlc::Array<std::shared_ptr<ast::Param>> cpp_parameter_strings_to_params(mlc::Array<mlc::String> parameter_strings) noexcept;

mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_fields_to_field_definitions(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept;

mlc::Array<std::shared_ptr<ast::TypeVariant>> cpp_enum_arms_to_variants(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> cpp_declaration_to_mlc_decls(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;

mlc::Array<std::shared_ptr<ast::Decl>> cpp_declarations_to_mlc_decls_list(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;

header_import::HeaderImportResult parse_cpp_header_source(mlc::String source_text) noexcept;

header_import::HeaderImportResult load_cpp_header_decls(mlc::String path) noexcept;

registry::TypeRegistry registry_from_cpp_header_source(mlc::String source_text) noexcept;

bool is_cpp_header_path(mlc::String path) noexcept{return path.length() >= 2 && path.substring(path.length() - 2, 2) == mlc::String(".h") || path.length() >= 4 && path.substring(path.length() - 4, 4) == mlc::String(".hpp");}

std::shared_ptr<ast::TypeExpr> cpp_type_string_to_type_expr(mlc::String type_string) noexcept{return [&]() -> std::shared_ptr<ast::TypeExpr> { if (type_string == mlc::String("int")) { return std::make_shared<ast::TypeExpr>((ast::TyI32{})); } if (type_string == mlc::String("bool")) { return std::make_shared<ast::TypeExpr>((ast::TyBool{})); } if (type_string == mlc::String("void")) { return std::make_shared<ast::TypeExpr>((ast::TyUnit{})); } return std::make_shared<ast::TypeExpr>(ast::TyNamed(type_string)); }();}

header_import::CppParameterParts split_cpp_parameter_string(mlc::String parameter_string) noexcept{
int last_space = -1;
int scan_index = 0;
while (scan_index < parameter_string.length()){
{
if (parameter_string.char_at(scan_index) == mlc::String(" ")){
{
last_space = scan_index;
}
}
scan_index = scan_index + 1;
}
}
return last_space < 0 ? header_import::CppParameterParts{parameter_string, mlc::String("")} : header_import::CppParameterParts{parameter_string.substring(0, last_space), parameter_string.substring(last_space + 1, parameter_string.length() - last_space - 1)};
}

mlc::Array<std::shared_ptr<ast::Param>> cpp_parameter_strings_to_params(mlc::Array<mlc::String> parameter_strings) noexcept{
mlc::Array<std::shared_ptr<ast::Param>> parameters = {};
int index = 0;
while (index < parameter_strings.size()){
{
header_import::CppParameterParts parts = split_cpp_parameter_string(parameter_strings[index]);
mlc::String parameter_name = parts.name == mlc::String("") ? mlc::String("arg") + mlc::to_string(index) : parts.name;
parameters.push_back(std::make_shared<ast::Param>(ast::Param{parameter_name, false, cpp_type_string_to_type_expr(parts.type_string), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown())), std::make_shared<ast::Pattern>(ast::PatternWild(ast::span_unknown()))}));
index = index + 1;
}
}
return parameters;
}

mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_fields_to_field_definitions(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept{
mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions = {};
int index = 0;
while (index < fields.size()){
{
std::shared_ptr<cpp_ast::CppField> field = fields[index];
field_definitions.push_back(std::make_shared<ast::FieldDef>(ast::FieldDef{field->name, cpp_type_string_to_type_expr(field->type_value), false, std::make_shared<ast::Expr>(ast::ExprUnit(ast::span_unknown()))}));
index = index + 1;
}
}
return field_definitions;
}

mlc::Array<std::shared_ptr<ast::TypeVariant>> cpp_enum_arms_to_variants(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept{
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants = {};
int index = 0;
while (index < arms.size()){
{
variants.push_back(std::make_shared<ast::TypeVariant>(ast::VarUnit(arms[index]->name, false)));
index = index + 1;
}
}
return variants;
}

mlc::Array<std::shared_ptr<ast::Decl>> cpp_declaration_to_mlc_decls(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{return [&]() -> mlc::Array<std::shared_ptr<ast::Decl>> { if (std::holds_alternative<cpp_ast::CppFnProto>((*declaration))) { auto _v_cppfnproto = std::get<cpp_ast::CppFnProto>((*declaration)); auto [_w0, return_type, function_name, parameter_strings] = _v_cppfnproto; return mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclFn(function_name, {}, {}, cpp_parameter_strings_to_params(parameter_strings), cpp_type_string_to_type_expr(return_type), std::make_shared<ast::Expr>(ast::ExprExtern(ast::span_unknown())), {}))}; } if (std::holds_alternative<cpp_ast::CppStruct>((*declaration))) { auto _v_cppstruct = std::get<cpp_ast::CppStruct>((*declaration)); auto [_w0, type_name, fields, _w1] = _v_cppstruct; return mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclType(type_name, {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, cpp_fields_to_field_definitions(fields), false))}, {}, ast::span_unknown()))}; } if (std::holds_alternative<cpp_ast::CppForwardDecl>((*declaration))) { auto _v_cppforwarddecl = std::get<cpp_ast::CppForwardDecl>((*declaration)); auto [_w0, type_name] = _v_cppforwarddecl; return mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclType(type_name, {}, mlc::Array<std::shared_ptr<ast::TypeVariant>>{std::make_shared<ast::TypeVariant>(ast::VarRecord(type_name, {}, false))}, {}, ast::span_unknown()))}; } if (std::holds_alternative<cpp_ast::CppUsing>((*declaration))) { auto _v_cppusing = std::get<cpp_ast::CppUsing>((*declaration)); auto [alias, type_string] = _v_cppusing; return mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclTypeAlias(alias, {}, cpp_type_string_to_type_expr(type_string), ast::span_unknown()))}; } if (std::holds_alternative<cpp_ast::CppVariant>((*declaration))) { auto _v_cppvariant = std::get<cpp_ast::CppVariant>((*declaration)); auto [_w0, enum_name, arms] = _v_cppvariant; return mlc::Array<std::shared_ptr<ast::Decl>>{std::make_shared<ast::Decl>(ast::DeclType(enum_name, {}, cpp_enum_arms_to_variants(arms), {}, ast::span_unknown()))}; } if (std::holds_alternative<cpp_ast::CppNamespace>((*declaration))) { auto _v_cppnamespace = std::get<cpp_ast::CppNamespace>((*declaration)); auto [_w0, inner_declarations] = _v_cppnamespace; return cpp_declarations_to_mlc_decls_list(inner_declarations); } return {}; }();}

mlc::Array<std::shared_ptr<ast::Decl>> cpp_declarations_to_mlc_decls_list(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept{
mlc::Array<std::shared_ptr<ast::Decl>> mlc_declarations = {};
int index = 0;
while (index < declarations.size()){
{
mlc::Array<std::shared_ptr<ast::Decl>> converted = cpp_declaration_to_mlc_decls(declarations[index]);
int inner_index = 0;
while (inner_index < converted.size()){
{
mlc_declarations.push_back(converted[inner_index]);
inner_index = inner_index + 1;
}
}
index = index + 1;
}
}
return mlc_declarations;
}

header_import::HeaderImportResult parse_cpp_header_source(mlc::String source_text) noexcept{
cpp_lexer::CppLexOut tokenize_result = cpp_lexer::cpp_tokenize(source_text);
cpp_parser::CppParseResult parse_result = cpp_parser::cpp_parse(tokenize_result.tokens);
return header_import::HeaderImportResult{cpp_declarations_to_mlc_decls_list(parse_result.program.declarations), parse_result.errors};
}

header_import::HeaderImportResult load_cpp_header_decls(mlc::String path) noexcept{
return !mlc::file::exists(path) ? header_import::HeaderImportResult{{}, mlc::Array<mlc::String>{mlc::String("file not found: ") + path}} : parse_cpp_header_source(mlc::file::read_to_string(path));
}

registry::TypeRegistry registry_from_cpp_header_source(mlc::String source_text) noexcept{return registry::build_registry(ast::Program{parse_cpp_header_source(source_text).declarations});}

} // namespace header_import
