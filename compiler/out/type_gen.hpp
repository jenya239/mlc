#ifndef TYPE_GEN_HPP
#define TYPE_GEN_HPP

#include "mlc.hpp"
#include <variant>

#include "ast.hpp"
#include "context.hpp"

namespace type_gen {

struct Expr;
struct Stmt;

mlc::String type_name_to_cpp(context::CodegenContext context, mlc::String type_name) noexcept;

mlc::String type_to_cpp(context::CodegenContext context, std::shared_ptr<ast::TypeExpr> type_expr) noexcept;

bool type_param_in_typeexpr(mlc::String param, std::shared_ptr<ast::TypeExpr> t) noexcept;

bool type_param_in_typeexpr_list(mlc::String param, mlc::Array<std::shared_ptr<ast::TypeExpr>> types) noexcept;

mlc::Array<std::shared_ptr<ast::TypeExpr>> variant_field_typeexprs(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::Array<mlc::String> variant_used_type_params(mlc::Array<mlc::String> type_params, std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String variant_ctor_name(std::shared_ptr<ast::TypeVariant> variant) noexcept;

mlc::String gen_adt_fwd(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_adt_defs(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_fwd_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String gen_type_decl_body_only(context::CodegenContext context, mlc::String type_name, mlc::Array<mlc::String> type_params, mlc::Array<std::shared_ptr<ast::TypeVariant>> variants) noexcept;

mlc::String requires_clause(mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds) noexcept;

} // namespace type_gen

#endif // TYPE_GEN_HPP
