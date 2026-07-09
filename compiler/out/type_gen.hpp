#ifndef TYPE_GEN_HPP
#define TYPE_GEN_HPP

#include "mlc.hpp"
#include <variant>
#include "ast.hpp"
#include "registry.hpp"
#include "record_field_default_validate.hpp"
#include "record_field_default_emit.hpp"
#include "context.hpp"
#include "decl_index.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"
namespace type_gen {

struct TupleStructCodegenFoldState {
  int field_slot_index;
  mlc::String declaration_text_so_far;
};
struct TupleHashCodegenFoldState {
  int tuple_field_slot;
  mlc::String concatenated_combine_lines;
};
struct HashSumOperatorFoldState {
  int next_discriminant_index;
  mlc::String operator_body_text;
};
mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;
mlc::String type_alias_annotation_cpp(context::CodegenContext context, mlc::String alias_name) noexcept;
mlc::String type_name_to_cpp_literal(mlc::String type_name) noexcept;
mlc::String type_name_to_cpp_self(context::CodegenContext context) noexcept;
mlc::String associated_type_name_cpp(context::CodegenContext context, mlc::String associated_type_name) noexcept;
mlc::String resolved_type_name_cpp(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> type_expression) noexcept;
bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> variant_used_type_parameters(mlc::Array<mlc::String> type_parameters, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> union_string_lists(mlc::Array<mlc::String> first, mlc::Array<mlc::String> second) noexcept;
mlc::Array<mlc::String> type_phantom_params_for_variants(mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String field_def_member_declaration(context::CodegenContext context, std::shared_ptr<ast::FieldDef> field_definition) noexcept;
mlc::String variant_record_struct_inline_member_declarations(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String tuple_variant_struct_members_fragment(context::CodegenContext context, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types) noexcept;
mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String struct_extra_using(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String variant_forward_decl_line(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String variant_type_argument_suffix(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String variant_alias_part(context::CodegenContext context, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String variant_def_with_prefix(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::String> phantom, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;
mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_definitions) noexcept;
mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_eq_sum(mlc::String type_name) noexcept;
mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_ord_sum(mlc::String type_name) noexcept;
mlc::String derive_hash_std_cpp_type(std::shared_ptr<ast::TypeExpr> field_type) noexcept;
mlc::String derive_hash_combine_line(mlc::String cpp_type, mlc::String access_expr) noexcept;
mlc::String derive_hash_record_operator_body(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_hash_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_hash_tuple_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::TypeExpr>> field_types, int discriminant_index) noexcept;
mlc::String derive_hash_record_variant_inner(mlc::String variant_struct_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs, int discriminant_index) noexcept;
mlc::String derive_hash_sum_variant_inner(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;
mlc::String type_variant_constructor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String derive_hash_sum_branch(std::shared_ptr<ast::TypeVariant> variant, int discriminant_index) noexcept;
mlc::String derive_hash_sum_operator_body(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_hash_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;
mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;
mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;
mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_parameters, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String requires_clause_bound_part(mlc::Array<mlc::String> type_parameters, int type_parameter_index, mlc::String bound_trait_name) noexcept;
mlc::String requires_clause(mlc::Array<mlc::String> type_parameters, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

} // namespace type_gen

#endif // TYPE_GEN_HPP
