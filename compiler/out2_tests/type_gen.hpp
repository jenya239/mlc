#ifndef TYPE_GEN_HPP
#define TYPE_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "registry.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "expr.hpp"

namespace type_gen {

mlc::String cpp_generic_base_name(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String sem_type_to_cpp(context::CodegenContext context, std::shared_ptr<registry::Type> semantic_type) noexcept;
mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept;
mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;
bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept;
bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;
mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_single_variant(context::CodegenContext context, mlc::String type_name, std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_field_display(mlc::String field_name, std::shared_ptr<ast::TypeExpr> field_type) noexcept;
mlc::String derive_display_record_fields(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_display_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_display_variant_case(std::shared_ptr<ast::TypeVariant> variant) noexcept;
mlc::String derive_display_variant_record(mlc::String name, mlc::Array<std::shared_ptr<ast::FieldDef>> fds) noexcept;
mlc::String derive_display_sum(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String derive_eq_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_eq_sum(mlc::String type_name) noexcept;
mlc::String derive_ord_build_cond(mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_ord_record(mlc::String type_name, mlc::Array<std::shared_ptr<ast::FieldDef>> field_defs) noexcept;
mlc::String derive_ord_sum(mlc::String type_name) noexcept;
bool variants_is_single_record(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::Array<std::shared_ptr<ast::FieldDef>> derive_record_field_defs(mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_derive_record_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;
mlc::String gen_derive_sum_trait(mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::String trait_name) noexcept;
mlc::String gen_derive_methods(context::CodegenContext context, mlc::String type_name, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants, mlc::Array<mlc::String> derive_traits) noexcept;
mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;
mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

} // namespace type_gen

#endif // TYPE_GEN_HPP
