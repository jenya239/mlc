#ifndef TYPE_INDEX_HPP
#define TYPE_INDEX_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "decl_index.hpp"
namespace type_index {

mlc::String extract_method_name(mlc::String fn_name, mlc::String type_name) noexcept;
mlc::String extract_owner_prefix_from_mangled(mlc::String fn_name) noexcept;
mlc::String trait_name_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String function_name_from_method_decl(std::shared_ptr<ast::Decl> method) noexcept;
bool extend_may_register_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String unmangled, mlc::String type_name, mlc::HashMap<mlc::String, bool> trait_names) noexcept;
void register_trait_name_from_decl(mlc::HashMap<mlc::String, bool>& trait_names, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::HashMap<mlc::String, bool> build_trait_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> decls) noexcept;
void register_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String owner_name, std::shared_ptr<ast::Decl> method) noexcept;
void register_trait_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String trait_name, mlc::Array<std::shared_ptr<ast::Decl>> methods) noexcept;
void register_extend_function_method_owner(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, std::shared_ptr<ast::Decl> method, mlc::HashMap<mlc::String, bool> trait_names) noexcept;
void register_extend_method_owners(mlc::HashMap<mlc::String, mlc::String>& owners, mlc::String type_name, mlc::Array<std::shared_ptr<ast::Decl>> methods, mlc::HashMap<mlc::String, bool> trait_names) noexcept;
mlc::String trait_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String extend_type_name_and_methods_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_trait_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::Decl>> methods_from_extend_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
void register_method_owners_from_decl(mlc::HashMap<mlc::String, mlc::String>& owners, std::shared_ptr<ast::Decl> declaration, mlc::HashMap<mlc::String, bool> trait_names) noexcept;
void add_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations, mlc::HashMap<mlc::String, mlc::String>& owners) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_method_owners_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;
mlc::String variant_name_from_type_variant(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String generic_variant_entry_name(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> type_parameters_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<mlc::String> append_generic_variant_names(mlc::Array<mlc::String> result, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<mlc::String> generic_variants_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<mlc::String> build_generic_variants_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;
mlc::String type_name_from_type_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
void register_variant_type_mapping(mlc::HashMap<mlc::String, mlc::String> variant_map, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
void register_variant_types_for_decl(mlc::HashMap<mlc::String, mlc::String> variant_map, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::HashMap<mlc::String, mlc::String> build_variant_types_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;
mlc::Array<mlc::String> field_names_from_field_definitions(mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_record_variant(mlc::String type_name, mlc::String variant_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions, int variant_count) noexcept;
mlc::String record_variant_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> record_field_definitions(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_for_type_variants(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
std::shared_ptr<ast::Decl> exported_inner_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::String type_name_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<ast::TypeVariant>> variants_from_top_level_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> field_orders_from_decl(std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::Array<std::shared_ptr<decl_index::FieldOrder>> build_field_orders(ast::Program program) noexcept;
bool type_param_in_typeexpr_for_index(mlc::String parameter_name, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool type_param_in_typeexpr_list_for_index(mlc::String parameter_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs_for_index(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> variant_used_type_parameters_for_index(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;
void register_sum_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> parameter_map, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_sum_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;
void register_variant_used_type_parameter_names(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, mlc::String type_name, mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;
void register_variant_used_type_parameter_names_for_decl(mlc::HashMap<mlc::String, mlc::Array<mlc::String>> used_map, std::shared_ptr<ast::Decl> declaration) noexcept;
mlc::HashMap<mlc::String, mlc::Array<mlc::String>> build_variant_used_type_parameter_names_from_decls(mlc::Array<std::shared_ptr<ast::Decl>> declarations) noexcept;

} // namespace type_index

#endif // TYPE_INDEX_HPP
