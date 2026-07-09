#ifndef DERIVE_METHODS_CPP_HPP
#define DERIVE_METHODS_CPP_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "cpp_ast.hpp"
#include "emit_helpers.hpp"
#include "hash_stmt_helpers.hpp"
namespace derive_methods_cpp {

std::shared_ptr<cpp_ast::CppExpression> make_string_constructor(mlc::String text) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_mlc_to_string_call(std::shared_ptr<cpp_ast::CppExpression> argument) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_self_field_expression(mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_self_discriminant_expression() noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_std_get_expression(mlc::String variant_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_holds_alternative_expression(mlc::String variant_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_field_display_expression(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_variant_field_display_expression(mlc::String variant_name, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> fold_plus_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_display_field_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_display_record_return_expression(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> append_variant_record_display_parts(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> parts, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_record_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_display_tuple_return_expression(mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_display_variant_return_expression(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
std::shared_ptr<cpp_ast::CppStatement> display_to_string_if_statement(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_definitions(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> empty_derive_field_definitions() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_display_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> display_to_string_function_definition(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_display_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_display_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_display_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_parameter_field_expression(mlc::String parameter_name, mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_parameter_discriminant_expression(mlc::String parameter_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> fold_and_expressions(mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> expressions) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppExpression>> build_eq_field_comparisons(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_eq_sum_return_expression() noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> eq_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_eq_sum_cpp(mlc::String type_name) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_eq_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_field_less_than_expression(mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_field_equal_expression(mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppExpression> build_prefix_equal_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int up_to_index) noexcept;
std::shared_ptr<cpp_ast::CppExpression> build_ord_record_return_expression(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppExpression> derive_ord_sum_return_expression() noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> ord_operator_function_definition(mlc::String type_name, std::shared_ptr<cpp_ast::CppExpression> return_expression) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_ord_sum_cpp(mlc::String type_name) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_ord_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_hash_tuple_field_access_expression(mlc::String variant_name, int field_index) noexcept;
std::shared_ptr<cpp_ast::CppExpression> make_hash_variant_field_access_expression(mlc::String variant_name, mlc::String field_name) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_combine_statement(mlc::String cpp_type, std::shared_ptr<cpp_ast::CppExpression> access) noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_seed_declaration_statement() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_empty_return_statement() noexcept;
std::shared_ptr<cpp_ast::CppStatement> make_hash_return_h_statement() noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_record_body_statements(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppStatement> derive_hash_discriminant_combine_statement(int discriminant_index) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_tuple_field_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> append_hash_record_variant_combine_statements(mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> statements, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int discriminant_index) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_variant_body_statements(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;
std::shared_ptr<cpp_ast::CppStatement> derive_hash_sum_branch_statement(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> derive_hash_sum_body_statements(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> hash_specialization_declaration(mlc::String type_name, mlc::Array<std::shared_ptr<cpp_ast::CppStatement>> body_statements) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_record_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
std::shared_ptr<cpp_ast::CppDeclaration> derive_hash_sum_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_hash_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_trait_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;
mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>> gen_derive_methods_cpp(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;

} // namespace derive_methods_cpp

#endif // DERIVE_METHODS_CPP_HPP
