#ifndef HEADER_IMPORT_HPP
#define HEADER_IMPORT_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_lexer.hpp"
#include "cpp_parser.hpp"
#include "cpp_ast.hpp"
#include "print.hpp"
#include "ast.hpp"
#include "registry.hpp"
namespace header_import {

struct HeaderImportResult {
  mlc::Array<std::shared_ptr<ast::Decl>> declarations;
  mlc::Array<mlc::String> errors;
};
struct CppParameterParts {
  mlc::String type_string;
  mlc::String name;
};
bool is_cpp_header_path(mlc::String path) noexcept;
std::shared_ptr<ast::TypeExpr> cpp_type_string_to_type_expr(mlc::String type_string) noexcept;
CppParameterParts split_cpp_parameter_string(mlc::String parameter_string) noexcept;
mlc::Array<std::shared_ptr<ast::Param>> cpp_parameter_strings_to_params(mlc::Array<mlc::String> parameter_strings) noexcept;
mlc::Array<std::shared_ptr<ast::Param>> cpp_cpp_parameters_to_mlc_params(mlc::Array<std::shared_ptr<cpp_ast::CppParam>> parameters) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> cpp_function_prototype_to_mlc_decls(cpp_ast::CppFunctionPrototype prototype) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_fields_to_field_definitions(mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields) noexcept;
mlc::Array<std::shared_ptr<ast::TypeVariant>> cpp_enum_arms_to_variants(mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms) noexcept;
mlc::String cpp_class_member_field_name(cpp_ast::CppClassMember member) noexcept;
std::shared_ptr<cpp_ast::CppType> cpp_class_member_field_type(cpp_ast::CppClassMember member) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> cpp_class_members_to_field_definitions(mlc::Array<cpp_ast::CppClassMember> members) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> cpp_declaration_to_mlc_decls(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> cpp_declarations_to_mlc_decls_list(mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations) noexcept;
HeaderImportResult parse_cpp_header_source(mlc::String source_text) noexcept;
HeaderImportResult load_cpp_header_decls(mlc::String path) noexcept;
registry::TypeRegistry registry_from_cpp_header_source(mlc::String source_text) noexcept;

} // namespace header_import

#endif // HEADER_IMPORT_HPP
