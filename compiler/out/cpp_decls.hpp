#ifndef CPP_DECLS_HPP
#define CPP_DECLS_HPP

#include "mlc.hpp"
#include <variant>
#include "cpp_types.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"
namespace cpp_decls {

struct CppDeclarationParseResult {
  std::shared_ptr<cpp_ast::CppDeclaration> declaration;
  int position;
  mlc::String error_message;
};
struct CppRecordFieldsParse {
  mlc::Array<std::shared_ptr<cpp_ast::CppField>> fields;
  int position;
};
struct CppParameterListParse {
  mlc::Array<mlc::String> parameters;
  int position;
};
struct CppEnumArmsParse {
  mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>> arms;
  int position;
};
struct CppDeclarationsBlockParse {
  mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> declarations;
  int position;
  mlc::Array<mlc::String> errors;
};
struct CppClassMembersParse {
  mlc::Array<cpp_ast::CppClassMember> members;
  int position;
};
struct CppTemplateHeaderParse {
  mlc::String parameters;
  int position;
};
CppDeclarationParseResult cpp_decl_empty_result(int position) noexcept;
int cpp_decl_skip_token(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_skip_to_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_after_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
bool cpp_decl_has_attribute_start(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_skip_one_attribute(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_skip_attributes(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_template_depth_after_token(int depth, cpp_tokens::CppTokenKind kind) noexcept;
int cpp_decl_template_skip_brackets(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, int depth) noexcept;
mlc::String cpp_decl_template_parameters_between(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;
CppTemplateHeaderParse cpp_decl_parse_template_header(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppClassMembersParse cpp_decl_parse_class_members(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_parse_include_skip(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_include_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_using_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppRecordFieldsParse cpp_decl_parse_record_fields(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult cpp_decl_parse_record_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String kind_word) noexcept;
CppDeclarationParseResult parse_cpp_struct_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_class_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppParameterListParse cpp_decl_parse_parameter_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_fn_proto_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppEnumArmsParse cpp_decl_parse_enum_arms(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_enum_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_next_scan_after_parse(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, CppDeclarationParseResult parsed) noexcept;
CppDeclarationsBlockParse parse_cpp_declarations_until_right_brace(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_namespace_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
bool cpp_decl_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept;
bool cpp_decl_can_start_fn_proto(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
bool cpp_decl_can_start_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
int cpp_decl_last_identifier_before_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int start) noexcept;
std::shared_ptr<cpp_ast::CppType> cpp_decl_type_node_until(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept;
CppDeclarationParseResult parse_cpp_typedef_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_template_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_extern_block_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;
CppDeclarationParseResult parse_cpp_declaration(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept;

} // namespace cpp_decls

#endif // CPP_DECLS_HPP
